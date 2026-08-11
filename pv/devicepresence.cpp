#include "devicepresence.hpp"

#include <tuple>

namespace pv {

bool UsbDeviceIdentity::operator<(const UsbDeviceIdentity &other) const
{
	return std::tie(vendor_id, product_id, bus_number, device_address) <
		std::tie(other.vendor_id, other.product_id,
			other.bus_number, other.device_address);
}

bool UsbDeviceIdentity::operator==(const UsbDeviceIdentity &other) const
{
	return vendor_id == other.vendor_id && product_id == other.product_id &&
		bus_number == other.bus_number &&
		device_address == other.device_address;
}

bool is_supported_analyzer(uint16_t vendor_id, uint16_t product_id)
{
	return vendor_id == 0x1a86 &&
		(product_id == 0x8025 || product_id == 0x5537 ||
			product_id == 0xfe17);
}

PresenceChange presence_change(
	const std::set<UsbDeviceIdentity> &before,
	const std::set<UsbDeviceIdentity> &after)
{
	if (before == after)
		return PresenceChange::Unchanged;
	if (before.empty())
		return PresenceChange::Added;
	if (after.empty())
		return PresenceChange::Removed;
	return PresenceChange::Replaced;
}

} // namespace pv
