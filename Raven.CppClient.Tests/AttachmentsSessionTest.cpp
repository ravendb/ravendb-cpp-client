#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "AdvancedSessionOperations.h"
#include "MetadataAsDictionary.h"
#include "AttachmentResult.h"
#include "DeleteAttachmentOperation.h"
#include "DeleteCommandData.h"
#include "AttachmentName.h"

namespace ravendb::client::tests::client::attachments
{
	class AttachmentsSessionTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(AttachmentsSessionTest, PutAttachments)
	{
		auto store = get_document_store(TEST_NAME);

		std::vector<std::string> names = { "profile.png", "background-photo.jpg", "fileNAME_#$1^%" };

		{
			auto session = store->open_session();

			constexpr size_t NUMBERS{ 1024 * 1024 };

			//using large attachment
			auto profile = std::vector<char>(NUMBERS, 'a');
			std::stringstream profile_stream{};
			profile_stream.write(profile.data(), profile.size());

			std::vector<char> background{ 10,20,30,40,50 };
			std::stringstream background_stream{};
			background_stream.write(background.data(), background.size());

			std::vector<char> file{ 1,2,3,4,5 };
			std::stringstream file_stream{};
			file_stream.write(file.data(), file.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment("users/1", names[0], profile_stream, "image/png");
			session.advanced().attachments()->store_attachment(user, names[1], background_stream, "ImAgE/jPeG");
			session.advanced().attachments()->store_attachment(user, names[2], file_stream);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<infrastructure::entities::User>("users/1");
			auto metadata = session.advanced().get_metadata_for(user);

			ASSERT_EQ("HasAttachments", std::any_cast<std::string>(
				metadata->get_dictionary().at(constants::documents::metadata::FLAGS)));

			auto attachments = std::any_cast<std::vector<std::any>>(
				metadata->get_dictionary().at(constants::documents::metadata::ATTACHMENTS));

			ASSERT_EQ(3, attachments.size());

			std::sort(names.begin(), names.end());

			size_t counter{ 0 };
			for(const auto& name : names)
			{
				auto&& attachment = std::static_pointer_cast<documents::session::IMetadataDictionary>(
					std::any_cast<std::shared_ptr<json::MetadataAsDictionary>>(
					attachments[counter++]));

				ASSERT_EQ(name, std::any_cast<std::string>(
					attachment->get_dictionary().at("Name")));
			}
		}
	}

	TEST_F(AttachmentsSessionTest, ThrowIfStreamIsUseTwice)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			std::vector<char> profile{ 1,2,3 };
			std::stringstream profile_stream{};
			profile_stream.write(profile.data(), profile.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment(user, "profile", profile_stream, "image/png");
			session.advanced().attachments()->store_attachment(user, "other", profile_stream);
			
			ASSERT_THROW({ session.save_changes(); }, std::runtime_error);
		}
	}

	TEST_F(AttachmentsSessionTest, ThrowWhenTwoAttachmentsWithTheSameNameInSession)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			std::vector<char> profile1{ 1,2,3 };
			std::stringstream profile_stream{};
			profile_stream.write(profile1.data(), profile1.size());

			std::vector<char> profile2{ 1,2,3 };
			std::stringstream profile_stream2{};
			profile_stream.write(profile2.data(), profile2.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment(user, "profile", profile_stream, "image/png");

			ASSERT_THROW(
				{
					session.advanced().attachments()->store_attachment(user, "profile", profile_stream2);
				}, std::runtime_error);
		}
	}

	TEST_F(AttachmentsSessionTest, PutDocumentAndAttachmentAndDeleteShouldThrow)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			std::vector<char> profile{ 1,2,3 };
			std::stringstream profile_stream{};
			profile_stream.write(profile.data(), profile.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment(user, "profile", profile_stream, "image/png");

			session.delete_document(user);

			ASSERT_THROW({ session.save_changes(); }, std::runtime_error);
		}
	}

	TEST_F(AttachmentsSessionTest, DeleteAttachments)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			std::vector<char> file1{ 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1.data(), file1.size());

			std::vector<char> file2{ 1,2,3,4,5,6 };
			std::stringstream file2_stream{};
			file2_stream.write(file2.data(), file2.size());

			std::vector<char> file3{ 1,2,3,4,5,6,7,8,9 };
			std::stringstream file3_stream{};
			file3_stream.write(file3.data(), file3.size());

			std::vector<char> file4{ 1,2,3,4,5,6,7,8,9,10,11,12 };
			std::stringstream file4_stream{};
			file4_stream.write(file4.data(), file4.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment(user, "file1", file1_stream, "image/png");
			session.advanced().attachments()->store_attachment(user, "file2", file2_stream, "image/png");
			session.advanced().attachments()->store_attachment(user, "file3", file3_stream, "image/png");
			session.advanced().attachments()->store_attachment(user, "file4", file4_stream, "image/png");


			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<infrastructure::entities::User>("users/1");

			//test get attachment by its name
			auto attachment_result = session.advanced().attachments()->get_attachment("users/1", "file2");
			ASSERT_EQ("file2", attachment_result->get_details().name);

			session.advanced().attachments()->delete_attachment("users/1", "file2");
			session.advanced().attachments()->delete_attachment(user, "file4");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<infrastructure::entities::User>("users/1");
			auto metadata = session.advanced().get_metadata_for(user);

			ASSERT_EQ("HasAttachments", std::any_cast<std::string>(
				metadata->get_dictionary().at(constants::documents::metadata::FLAGS)));

			auto attachments = std::any_cast<std::vector<std::any>>(
				metadata->get_dictionary().at(constants::documents::metadata::ATTACHMENTS));

			ASSERT_EQ(2, attachments.size());

			auto result = session.advanced().attachments()->get_attachment("users/1", "file1");
			std::stringstream res{};
			res << result->get_data().rdbuf();
			ASSERT_EQ(3, res.str().size());
		}
	}

	TEST_F(AttachmentsSessionTest, DeleteAttachmentsUsingCommand)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			std::vector<char> file1{ 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1.data(), file1.size());

			std::vector<char> file2{ 1,2,3,4,5,6 };
			std::stringstream file2_stream{};
			file2_stream.write(file2.data(), file2.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment(user, "file1", file1_stream, "image/png");
			session.advanced().attachments()->store_attachment(user, "file2", file2_stream, "image/png");

			session.save_changes();
		}
		auto operation = documents::operations::attachments::DeleteAttachmentOperation("users/1", "file2");
		store->operations()->send(operation);

		{
			auto session = store->open_session();

			auto user = session.load<infrastructure::entities::User>("users/1");
			auto metadata = session.advanced().get_metadata_for(user);

			ASSERT_EQ("HasAttachments", std::any_cast<std::string>(
				metadata->get_dictionary().at(constants::documents::metadata::FLAGS)));

			auto attachments = std::any_cast<std::vector<std::any>>(
				metadata->get_dictionary().at(constants::documents::metadata::ATTACHMENTS));

			ASSERT_EQ(1, attachments.size());

			auto result = session.advanced().attachments()->get_attachment("users/1", "file1");
			std::stringstream res{};
			res << result->get_data().rdbuf();
			ASSERT_EQ(3, res.str().size());
		}
	}

	TEST_F(AttachmentsSessionTest, GetAttachmentReleasesResources)
	{
		constexpr size_t count{ 30 };

		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.save_changes();
		}

		for (size_t i = 0; i < count; ++i)
		{
			auto session = store->open_session();
			std::vector<char> file{ 1,2,3 };
			std::stringstream file_stream{};
			file_stream.write(file.data(), file.size());
			session.advanced().attachments()->store_attachment("users/1", "file" + std::to_string(i),
				file_stream, "image/png");
			session.save_changes();
		}

		for (size_t i = 0; i < count; ++i)
		{
			auto session = store->open_session();
			auto result = session.advanced().attachments()->get_attachment("users/1", "file" + std::to_string(i));
		}
	}

	TEST_F(AttachmentsSessionTest, DeleteDocumentAndThanItsAttachments_ThisIsNoOpButShouldBeSupported)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			std::vector<char> file1{ 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1.data(), file1.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment(user, "file", file1_stream, "image/png");

			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto user = session.load<infrastructure::entities::User>("users/1");

			session.delete_document(user);
			session.advanced().attachments()->delete_attachment(user, "file");
			session.advanced().attachments()->delete_attachment(user, "file"); //this should be no-op

			session.save_changes();
		}
	}

	TEST_F(AttachmentsSessionTest, DeleteDocumentByCommandAndThanItsAttachments_ThisIsNoOpButShouldBeSupported)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			std::vector<char> file1{ 1,2,3 };
			std::stringstream file1_stream{};
			file1_stream.write(file1.data(), file1.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment(user, "file", file1_stream, "image/png");

			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto user = session.load<infrastructure::entities::User>("users/1");

			session.advanced().defer({ std::make_shared<documents::commands::batches::DeleteCommandData>(
				"users/1", "") });
			session.advanced().attachments()->delete_attachment("users/1", "file");
			session.advanced().attachments()->delete_attachment("users/1", "file");

			session.save_changes();
		}
	}

	TEST_F(AttachmentsSessionTest, GetAttachmentNames)
	{
		auto store = get_document_store(TEST_NAME);

		std::vector<std::string> names = { "profile.png" };

		{
			auto session = store->open_session();

			std::vector<char> profile{ 1,2,3 };
			std::stringstream profile_stream{};
			profile_stream.write(profile.data(), profile.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment("users/1", names[0], profile_stream, "image/png");

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<infrastructure::entities::User>("users/1");
			auto attachments = session.advanced().attachments()->get_attachments_names(user);

			ASSERT_EQ(1, attachments.size());

			const auto& attachment = attachments[0];

			ASSERT_EQ("image/png", attachment.content_type);
			ASSERT_EQ(names[0], attachment.name);
			ASSERT_EQ(3, attachment.size);
		}
	}

	TEST_F(AttachmentsSessionTest, AttachmentExists)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			std::vector<char> profile{ 1,2,3 };
			std::stringstream profile_stream{};
			profile_stream.write(profile.data(), profile.size());

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");

			session.advanced().attachments()->store_attachment("users/1", "profile", profile_stream, "image/png");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			ASSERT_TRUE(session.advanced().attachments()->exists("users/1", "profile"));
			ASSERT_FALSE(session.advanced().attachments()->exists("users/1", "background-photo"));
			ASSERT_FALSE(session.advanced().attachments()->exists("users/2", "profile"));
		}
	}
}
