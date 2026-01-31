#include <sw/debug_formatter.hpp>
#include <print>
#include <string>
#include <format>
#include <stdexcept>
#include "utils.hpp"

namespace test_ns
{
struct Point
{
    int x, y;
};

struct Person
{
    std::string name;
    int age;
};
}

// Enable debug formatting for the types
SW_DERIVE_DEBUG(test_ns::Point);
SW_DERIVE_DEBUG(test_ns::Person);

void run_tests()
{
    // Test 1: Basic struct formatting
    {
        test_ns::Point p{10, 20};
        // Expect: "Point @ <Address>"
        std::string s = std::format("{:?}", p);

        // Verify type name is present
        ASSERT_TRUE(s.find("Point") != std::string::npos);
        // Verify address marker is present
        ASSERT_TRUE(s.find("@") != std::string::npos);

        // Verify output is not empty
        ASSERT_TRUE(!s.empty());
    }

    // Test 2: Another struct with different data
    {
        test_ns::Person person{"Alice", 30};
        std::string s = std::format("{:?}", person);

        ASSERT_TRUE(s.find("Person") != std::string::npos);
        ASSERT_TRUE(s.find("@") != std::string::npos);
    }

    // Test 3: Exception on missing '?'
    {
        test_ns::Point p{0, 0};
        bool caught = false;
        try
        {
            // This should throw because SW_DERIVE_DEBUG enforces usage of {:?}
            // and the implementation explicitly checks for '?' in parse().
            // Use vformat to bypass compile-time check
            auto s = std::vformat("{}", std::make_format_args(p));
            (void)s;
        }
        catch (const std::format_error& e)
        {
            caught = true;
            std::string msg = e.what();
            // Check for part of the error message defined in debug_formatter.hpp
            ASSERT_TRUE(msg.find("missing '?'") != std::string::npos);
        }
        catch (...)
        {
            // Should catch specific type, but failing safely if unknown
        }
        ASSERT_TRUE(caught);
    }
}

TEST_MAIN
