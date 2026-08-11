#ifndef PULSEVIEW_PV_DEVICEPRESENCE_HPP
#define PULSEVIEW_PV_DEVICEPRESENCE_HPP

#include <cstdint>
#include <set>

namespace pv {

struct UsbDeviceIdentity
{
	uint16_t vendor_id;
	uint16_t product_id;
	uint8_t bus_number;
	uint8_t device_address;

	bool operator<(const UsbDeviceIdentity &other) const;
	bool operator==(const UsbDeviceIdentity &other) const;
};

enum class PresenceChange
{
	Unchanged,
	Added,
	Removed,
	Replaced
};

bool is_supported_analyzer(uint16_t vendor_id, uint16_t product_id);

PresenceChange presence_change(
	const std::set<UsbDeviceIdentity> &before,
	const std::set<UsbDeviceIdentity> &after);

} // namespace pv

#endif // PULSEVIEW_PV_DEVICEPRESENCE_HPP
