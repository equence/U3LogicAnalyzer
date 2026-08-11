/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2015 Joel Holdsworth <joel@airwebreathe.org.uk>
 * Copyright (C) 2026 Q2H2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>

#include <QTimer>
#include <QToolTip>
#include <QDebug>
#include <libsigrokcxx/libsigrokcxx.hpp>

#include <pv/devicemanager.hpp>
#include <pv/devices/device.hpp>

#include "devicetoolbutton.hpp"

using std::list;
using std::shared_ptr;
using std::string;
using std::weak_ptr;
using std::vector;

using pv::devices::Device;

namespace pv {
namespace widgets {

static QString concise_device_name(const QString &vendor)
{
	if (vendor == "USB3.0(CH32H417)")
		return "CH32H417";
	if (vendor == "USB3.0(CH569)")
		return "CH569";
	return vendor;
}

DeviceToolButton::DeviceToolButton(QWidget *parent,
	DeviceManager *device_manager,
	QAction *connect_action) :
	QToolButton(parent),
	device_manager_(device_manager),
	connect_action_(connect_action),
	menu_(this),
	mapper_(this),
	devices_()
{
	setPopupMode(QToolButton::InstantPopup);
	setMenu(&menu_);
	setToolTip("");
	setObjectName("deviceSelector");
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	setIcon(QIcon(":/icons/status-grey.svg"));
	setIconSize(QSize(10, 10));
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	menu_.setObjectName("deviceMenu");
	menu_.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint |
		Qt::NoDropShadowWindowHint);
	menu_.setAttribute(Qt::WA_TranslucentBackground, true);
	menu_.setStyleSheet(
		"QMenu#deviceMenu { padding: 4px; background: #25282e;"
		" border: 1px solid #414650; color: #dce1e8; font-size: 12px; }"
		"QMenu#deviceMenu::item { height: 28px; padding: 0 14px;"
		" margin: 0; border-radius: 5px; background: transparent; }"
		"QMenu#deviceMenu::item:selected { background: #343943; color: #ffffff; }"
	);

	connect(&mapper_, SIGNAL(mapped(QObject*)),
		this, SLOT(on_action(QObject*)));

	connect(&menu_, SIGNAL(hovered(QAction*)),
		this, SLOT(on_menu_hovered(QAction*)));

	setMinimumWidth(QFontMetrics(font()).averageCharWidth() * 22);
	setFixedHeight(32);
}

shared_ptr<Device> DeviceToolButton::selected_device()
{
	return selected_device_;
}

QString DeviceToolButton::selected_vendor() const
{
	return selected_vendor_;
}

void DeviceToolButton::set_device_list(
	const list< shared_ptr<Device> > &devices, shared_ptr<Device> selected)
{
	selected_device_ = selected;
	selected_vendor_ = selected ? QString::fromStdString(
		selected->device()->vendor()) : QString();
	setIcon(QIcon(selected ? ":/icons/status-green.svg" :
		":/icons/status-grey.svg"));
	setText(selected ? QString::fromStdString(
		selected->display_name(*device_manager_)) : tr("Not connected"));
	devices_ = vector< weak_ptr<Device> >(devices.begin(), devices.end());
	update_device_list();
}

void DeviceToolButton::reset()
{
	set_not_connected();
}

void DeviceToolButton::set_not_connected()
{
	selected_device_.reset();
	selected_vendor_.clear();
	setIcon(QIcon(":/icons/status-grey.svg"));
	devices_.clear();
	menu_.clear();
	setText(tr("Not connected"));
}

void DeviceToolButton::set_device_list_name(const QString &device_list_name)
{
	setText(device_list_name);
	update_device_list();
}

void DeviceToolButton::update_device_list()
{
	menu_.clear();
	menu_.setMinimumWidth(width());

	for (weak_ptr<Device>& dev_weak_ptr : devices_) {
		shared_ptr<Device> dev(dev_weak_ptr.lock());
		if (!dev)
			continue;

		const QString vendor = QString::fromStdString(dev->device()->vendor());
		const QString label = (selected_device_ == dev && !text().isEmpty()) ?
			text() : concise_device_name(vendor);
		QAction *const a = new QAction(label, &menu_);
		a->setData(QVariant::fromValue((void*)dev.get()));
		a->setProperty("deviceVendor", vendor);
		a->setToolTip(QString::fromStdString(dev->full_name()));
		mapper_.setMapping(a, a);

		connect(a, SIGNAL(triggered()), &mapper_, SLOT(map()));

		menu_.addAction(a);
	}
}

void DeviceToolButton::update_device_mamager(DeviceManager * dev_manager)
{
	device_manager_ = dev_manager;
}

void DeviceToolButton::on_action(QObject *action)
{
	assert(action);

	QAction *selected_action = qobject_cast<QAction*>(action);
	Device *const dev = (Device*)selected_action->data().value<void*>();
	if (selected_device_.get() == dev) {
		setText(selected_action->text());
		return;
	}

	selected_device_.reset();
	for (weak_ptr<Device>& dev_weak_ptr : devices_) {
		shared_ptr<Device> dev_ptr(dev_weak_ptr);
		if (dev_ptr.get() == dev) {
			selected_device_ = shared_ptr<Device>(dev_ptr);
			break;
		}
	}
	if (!selected_device_) {
		set_not_connected();
		return;
	}

	selected_vendor_ = selected_action->property("deviceVendor").toString();
	setText(selected_action->text());
	device_selected();
}

void DeviceToolButton::on_menu_hovered(QAction *action)
{
	assert(action);

	// Only show the tooltip for device entries (they hold
	// device pointers in their data field)
	if (!action->data().isValid())
		return;

	device_tooltip_ = action->toolTip();

	if (QToolTip::isVisible())
		on_menu_hover_timeout();
	else
		QTimer::singleShot(1000, this, SLOT(on_menu_hover_timeout()));
}

void DeviceToolButton::on_menu_hover_timeout()
{
	if (device_tooltip_.isEmpty())
		return;

	QToolTip::showText(QCursor::pos(), device_tooltip_);
}

}  // namespace widgets
}  // namespace pv
