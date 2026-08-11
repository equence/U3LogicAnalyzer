#include <boost/test/unit_test.hpp>

#include "pv/softwareupdate.hpp"

BOOST_AUTO_TEST_SUITE(SoftwareUpdateTest)

BOOST_AUTO_TEST_CASE(compares_numeric_version_components)
{
	BOOST_CHECK(pv::softwareupdate::is_newer_version("v0.5", "0.4"));
	BOOST_CHECK(pv::softwareupdate::is_newer_version("0.10", "0.9"));
	BOOST_CHECK(!pv::softwareupdate::is_newer_version("v0.4", "0.4.0"));
	BOOST_CHECK(!pv::softwareupdate::is_newer_version("0.3", "0.4"));
}

BOOST_AUTO_TEST_CASE(parses_latest_release_response)
{
	pv::softwareupdate::ReleaseInfo release;
	const QByteArray json = R"({
		"tag_name":"v0.5",
		"name":"U3LogicAnalyzer v0.5",
		"html_url":"https://github.com/equence/U3LogicAnalyzer/releases/tag/v0.5"
	})";
	BOOST_REQUIRE(pv::softwareupdate::parse_github_release(json, &release));
	BOOST_CHECK_EQUAL(release.tag_name.toStdString(), "v0.5");
	BOOST_CHECK_EQUAL(release.name.toStdString(), "U3LogicAnalyzer v0.5");
}

BOOST_AUTO_TEST_CASE(rejects_incomplete_release_response)
{
	pv::softwareupdate::ReleaseInfo release;
	BOOST_CHECK(!pv::softwareupdate::parse_github_release(
		QByteArray("{\"tag_name\":\"v0.5\"}"), &release));
}

BOOST_AUTO_TEST_SUITE_END()
