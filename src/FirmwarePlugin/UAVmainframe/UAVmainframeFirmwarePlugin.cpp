/*=====================================================================

 QGroundControl Open Source Ground Control Station
 
 (c) 2009 - 2015 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 
 This file is part of the QGROUNDCONTROL project
 
 QGROUNDCONTROL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 QGROUNDCONTROL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.
 
 ======================================================================*/

/// @file
///     @author Don Gagne <don@thegagnes.com>

#include "UAVmainframeFirmwarePlugin.h"

#include <QDebug>

IMPLEMENT_QGC_SINGLETON(UAVmainframeFirmwarePlugin, FirmwarePlugin)

UAVmainframeFirmwarePlugin::UAVmainframeFirmwarePlugin(QObject* parent) :
    FirmwarePlugin(parent)
{
    
}

QList<VehicleComponent*> UAVmainframeFirmwarePlugin::componentsForVehicle(AutoPilotPlugin* vehicle)
{
    Q_UNUSED(vehicle);
    
    return QList<VehicleComponent*>();
}

QString UAVmainframeFirmwarePlugin::flightMode(uint8_t base_mode, uint32_t custom_mode)
{
    QString flightMode;
    
    struct Bit2Name {
        uint8_t     baseModeBit;
        const char* name;
    };
    static const struct Bit2Name rgBit2Name[] = {
    { MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,   "Manual" },
    { MAV_MODE_FLAG_STABILIZE_ENABLED,      "Stabilize" },
    { MAV_MODE_FLAG_GUIDED_ENABLED,         "Guided" },
    { MAV_MODE_FLAG_AUTO_ENABLED,           "Auto" },
    { MAV_MODE_FLAG_TEST_ENABLED,           "Test" },
};

    struct Bit2NameCustom {
        uint32_t     customModeBit;
        const char* name;
    };
    static const struct Bit2NameCustom rgBit2NameCustom[] = {
    { UAVmainframe_MODE_PREFLIGHT,          "PreFlight"},
    { UAVmainframe_MODE_MANUAL,             "Manual" },
    { UAVmainframe_MODE_PASSTHROUGH,        "PassThrough" },
    { UAVmainframe_MODE_STABILISE,          "Stabilise" },
    { UAVmainframe_MODE_TECS,               "Tecs" },
    { UAVmainframe_MODE_AUTO,               "Auto" },
    { UAVmainframe_MODE_INPUT,              "Input" },
    { UAVmainframe_MODE_FLAG_FLIGHT_SIM,    "Sim" },
};
    
    
    if (base_mode == 0 || custom_mode == 0)
    {
        flightMode = "PreFlight";
    }

    else if (base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED)
    {
        //flightMode = QString("Custom:0x%1").arg(custom_mode, 0, 16);

        for (size_t i=0; i<sizeof(rgBit2NameCustom)/sizeof(rgBit2NameCustom[0]); i++) {
            if (custom_mode & rgBit2NameCustom[i].customModeBit) {
                if (i != 0) {
                    flightMode += " ";
                }
                flightMode += rgBit2NameCustom[i].name;
            }
        }
    }

    else
    {
        for (size_t i=0; i<sizeof(rgBit2Name)/sizeof(rgBit2Name[0]); i++) {
            if (base_mode & rgBit2Name[i].baseModeBit) {
                if (i != 0) {
                    flightMode += " ";
                }
                flightMode += rgBit2Name[i].name;
            }
        }
    }
    
    return flightMode;
}

bool UAVmainframeFirmwarePlugin::setFlightMode(const QString& flightMode, uint8_t* base_mode, uint32_t* custom_mode)
{
    Q_UNUSED(flightMode);
    Q_UNUSED(base_mode);
    Q_UNUSED(custom_mode);
    
    qWarning() << "UAVmainframeFirmwarePlugin::setFlightMode called on base class, not supported";
    
    return false;
}
