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
    
    // Base Modes
    struct Bit2Name_Base {
        uint8_t     baseModeBit;
        const char* name;
    };
    static const struct Bit2Name_Base rgBit2Name[] = {
    { MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,   "Manual" },
    { MAV_MODE_FLAG_STABILIZE_ENABLED,      "Stabilize" },
    { MAV_MODE_FLAG_GUIDED_ENABLED,         "Guided" },
    { MAV_MODE_FLAG_AUTO_ENABLED,           "Auto" },
    { MAV_MODE_FLAG_TEST_ENABLED,           "Test" },
};

    // UAVmainframe custom modes and flags
    union {
        struct{
            uint16_t mode;
            uint8_t sub_mode;
            uint8_t mode_flags;
        };
        uint32_t raw;
    }rx_data;

    struct Bit2Name_Custom {
        uint16_t     ModeBit;
        uint16_t     SubModeBit;
        const char* name;
    };
    static const struct Bit2Name_Custom Bit2Name[] = {
    { UAVmainframe_MODE_PREFLIGHT          ,0                                      ,"PreFlight"},
    { UAVmainframe_MODE_MANUAL             ,0                                      ,"Manual" },
    { UAVmainframe_MODE_PASSTHROUGH        ,0                                      ,"PassThrough" },
    { UAVmainframe_MODE_STABILISE          ,0                                      ,"Stabilise" },
    { UAVmainframe_MODE_TECS               ,0                                      ,"Tecs" },
    { UAVmainframe_MODE_AUTO               ,UAVmainframe_SUB_MODE_RETURN_HOME      ,"Return home" },
    { UAVmainframe_MODE_INPUT              ,0                                      ,"Random Input" },
    { UAVmainframe_MODE_INPUT              ,UAVmainframe_SUB_MODE_FLAG_ELEVATOR    ,"Elevator Input" },
    { UAVmainframe_MODE_INPUT              ,UAVmainframe_SUB_MODE_FLAG_RUDDER      ,"Rudder Input" },
    { UAVmainframe_MODE_INPUT              ,UAVmainframe_SUB_MODE_FLAG_AILERON     ,"Aileron Input" },
    { UAVmainframe_MODE_INPUT              ,UAVmainframe_SUB_MODE_FLAG_THROTTLE    ,"Throttle Input" },
    { UAVmainframe_MODE_INPUT              ,UAVmainframe_SUB_MODE_FLAG_GLIDE       ,"Glide" },
    { UAVmainframe_MODE_INPUT              ,UAVmainframe_SUB_MODE_FLAG_ACCEL       ,"Accel" },
    { UAVmainframe_MODE_INPUT              ,UAVmainframe_SUB_MODE_FLAG_TURN        ,"Turn" },
};
    
    
    if (base_mode == 0 || custom_mode == 0)
    {
        flightMode = "PreFlight";
    }

    else if (base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED)
    {
        //flightMode = QString("Custom:0x%1").arg(custom_mode, 0, 16);

        // copy raw data into union to decode components
        rx_data.raw = custom_mode;

        // search for mode and sub mode
        for (size_t i=0; i<sizeof(Bit2Name)/sizeof(Bit2Name[0]); i++)
        {
            const struct Bit2Name_Custom* Bit2NameCurrent = &Bit2Name[i];

            //qWarning() << rx_mode.mode << rx_mode.sub_mode;

            if ((rx_data.mode == Bit2NameCurrent->ModeBit) && (rx_data.sub_mode == Bit2NameCurrent->SubModeBit))
            {
                flightMode = Bit2NameCurrent->name;
                break;
            }
        }

        // closed loop input flag set?
        if(rx_data.mode_flags & UAVmainframe_MODE_FLAG_INPUT_CLOSED_LOOP)
        {
            flightMode += " Closed";
        }

        // flight sim flag set?
        if(rx_data.mode_flags & UAVmainframe_MODE_FLAG_FLIGHT_SIM)
        {
            flightMode += " Sim";
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
