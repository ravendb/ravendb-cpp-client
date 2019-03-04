#pragma once

namespace ravendb::client::constants
{
	namespace documents
	{
		namespace metadata
		{
			constexpr char COLLECTION[] = "@collection";
			constexpr char PROJECTION[] = "@projection";
			constexpr char KEY[] = "@metadata";
			constexpr char ID[] = "@id";
			constexpr char CONFLICT[] = "@conflict";
			constexpr char ID_PROPERTY[] = "Id";
			constexpr char FLAGS[] = "@flags";
			constexpr char ATTACHMENTS[] = "@attachments";
			constexpr char COUNTERS[] = "@counters";
			constexpr char REVISION_COUNTERS[] = "@counters-snapshot";
			constexpr char INDEX_SCORE[] = "@index-score";
			constexpr char LAST_MODIFIED[] = "@last-modified";
			constexpr char RAVEN_C_PLUS_PLUS_TYPE[] = "Raven-C-Plus-Plus-Type";
			constexpr char CHANGE_VECTOR[] = "@change-vector";
			constexpr char EXPIRES[] = "@expires";
			constexpr char ALL_DOCUMENTS_COLLECTION[] = "@all_docs";
		}
		namespace indexing
		{
			constexpr char SIDE_BY_SIDE_INDEX_NAME_PREFIX[] = "ReplacementOf/";

			namespace fields
			{
				constexpr char DOCUMENT_ID_FIELD_NAME[] = "id()";
				constexpr char REDUCE_KEY_HASH_FIELD_NAME[] = "hash(key())";
				constexpr char REDUCE_KEY_KEY_VALUE_FIELD_NAME[] = "key()";
				constexpr char ALL_FIELDS[] = "__all_fields";
				constexpr char SPATIAL_SHAPE_FIELD_NAME[] = "spatial(shape)";
			}
			namespace spacial
			{
				constexpr double DEFAULT_DISTANCE_ERROR_PCT = 0.025;
			}
		}
		namespace periodic_backup
		{
			constexpr char FULL_BACKUP_EXTENSION[] = "ravendb-full-backup";
			constexpr char SNAPSHOT_EXTENSION[] = "ravendb-snapshot";
			constexpr char INCREMENTAL_BACKUP_EXTENSION[] = "ravendb-incremental-backup";

			namespace folders
			{
				constexpr char INDEXES[] = "Indexes";
				constexpr char DOCUMENTS[] = "Documents";
				constexpr char CONFIGURATION[] = "Configuration";
			}
		}
	}
	namespace headers
	{
		constexpr char REQUEST_TIME[] = "Raven-Request-Time";

		constexpr char REFRESH_TOPOLOGY[] = "Refresh-Topology";

		constexpr char TOPOLOGY_ETAG[] = "Topology-Etag";

		constexpr char LAST_KNOWN_CLUSTER_TRANSACTION_INDEX[] = "Known-Raft-Index";

		constexpr char CLIENT_CONFIGURATION_ETAG[] = "Client-Configuration-Etag";

		constexpr char REFRESH_CLIENT_CONFIGURATION[] = "Refresh-Client-Configuration";

		constexpr char CLIENT_VERSION[] = "Raven-Client-Version";
		constexpr char SERVER_VERSION[] = "Raven-Server-Version";

		constexpr char ETAG[] = "ETag";

		constexpr char IF_NONE_MATCH[] = "If-None-Match";
		constexpr char TRANSFER_ENCODING[] = "Transfer-Encoding";
		constexpr char CONTENT_ENCODING[] = "Content-Encoding";
		constexpr char CONTENT_LENGTH[] = "Content-Length";
	}
	namespace counters
	{
		constexpr char ALL[] = "@all_counters";
	}
}
