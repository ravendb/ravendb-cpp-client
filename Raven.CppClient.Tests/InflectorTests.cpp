#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "Inflector.h"

using namespace ravendb::client::impl;
namespace ravendb::client::tests::client {

    TEST(InflectorTests, PluralizeWolf)
    {
        ASSERT_EQ("Wolves", Inflector::pluralize("Wolf"));
    }

    TEST(InflectorTests, PluralizeBacterium)
    {
        ASSERT_EQ("bacteria", Inflector::pluralize("bacterium"));
    }

    TEST(InflectorTests, PluralizeChief)
    {
        ASSERT_EQ("chiefs", Inflector::pluralize("chief"));
    }

    TEST(InflectorTests, PluralizeBox)
    {
        ASSERT_EQ("boxes", Inflector::pluralize("box"));
    }

    TEST(InflectorTests,PluralizeUser)
    {
        ASSERT_EQ("Users", Inflector::pluralize("User"));
    }

    TEST(InflectorTests,PluralizeUsers)
    {
        ASSERT_EQ("Users", Inflector::pluralize("Users"));
    }

    TEST(InflectorTests,PluralizeTanimport)
    {
        ASSERT_EQ("tanimports", Inflector::pluralize("tanimport"));
    }

    TEST(InflectorTests,PluralizeTanimports)
    {
        ASSERT_EQ("tanimports", Inflector::pluralize("tanimports"));
    }
}