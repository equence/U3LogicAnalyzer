#include <boost/test/unit_test.hpp>

#include <set>

#include "pv/devicepresence.hpp"

using pv::PresenceChange;
using pv::UsbDeviceIdentity;

BOOST_AUTO_TEST_SUITE(DevicePresenceTest)

BOOST_AUTO_TEST_CASE(supported_wch_analyzers_are_recognized)
{
	BOOST_CHECK(pv::is_supported_analyzer(0x1a86, 0x8025));
	BOOST_CHECK(pv::is_supported_analyzer(0x1a86, 0x5537));
	BOOST_CHECK(pv::is_supported_analyzer(0x1a86, 0xfe17));
	BOOST_CHECK(!pv::is_supported_analyzer(0x1a86, 0x0001));
	BOOST_CHECK(!pv::is_supported_analyzer(0x1234, 0x5537));
}

BOOST_AUTO_TEST_CASE(equal_snapshots_do_not_trigger_refresh)
{
	const std::set<UsbDeviceIdentity> devices = {
		{0x1a86, 0x5537, 1, 2}
	};
	BOOST_CHECK(pv::presence_change(devices, devices) == PresenceChange::Unchanged);
}

BOOST_AUTO_TEST_CASE(first_insertion_is_detected)
{
	const std::set<UsbDeviceIdentity> before;
	const std::set<UsbDeviceIdentity> after = {
		{0x1a86, 0x5537, 1, 2}
	};
	BOOST_CHECK(pv::presence_change(before, after) == PresenceChange::Added);
}

BOOST_AUTO_TEST_CASE(removal_is_detected)
{
	const std::set<UsbDeviceIdentity> before = {
		{0x1a86, 0x5537, 1, 2}
	};
	const std::set<UsbDeviceIdentity> after;
	BOOST_CHECK(pv::presence_change(before, after) == PresenceChange::Removed);
}

BOOST_AUTO_TEST_CASE(replacement_is_detected)
{
	const std::set<UsbDeviceIdentity> before = {
		{0x1a86, 0x5537, 1, 2}
	};
	const std::set<UsbDeviceIdentity> after = {
		{0x1a86, 0x8025, 1, 3}
	};
	BOOST_CHECK(pv::presence_change(before, after) == PresenceChange::Replaced);
}

BOOST_AUTO_TEST_SUITE_END()
