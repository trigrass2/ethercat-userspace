/*****************************************************************************
 *
 *  $Id: CommandGraph.cpp,v 4f682084c643 2010/10/25 08:12:26 fp $
 *
 *  Copyright (C) 2006-2009  Florian Pose, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherCAT Master.
 *
 *  The IgH EtherCAT Master is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License version 2, as
 *  published by the Free Software Foundation.
 *
 *  The IgH EtherCAT Master is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *  Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with the IgH EtherCAT Master; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  ---
 *
 *  The license mentioned above concerns the source code only. Using the
 *  EtherCAT technology and brand is only permitted in compliance with the
 *  industrial property and similar rights of Beckhoff Automation GmbH.
 *
 ****************************************************************************/

#include <iostream>
#include <map>
using namespace std;

#include "CommandGraph.h"
#include "MasterDevice.h"

/*****************************************************************************/

CommandGraph::CommandGraph():
    Command("graph", "Output the bus topology as a graph.")
{
}

/*****************************************************************************/

string CommandGraph::helpString(const string &binaryBaseName) const
{
    stringstream str;

    str << binaryBaseName << " " << getName() << " [OPTIONS]" << endl
        << endl
        << getBriefDescription() << endl
        << endl
        << "The bus is output in DOT language (see" << endl
        << "http://www.graphviz.org/doc/info/lang.html), which can" << endl
        << "be processed with the tools from the Graphviz" << endl
        << "package. Example:" << endl
        << endl
        << "  ethercat graph | dot -Tsvg > bus.svg" << endl
        << endl
        << "See 'man dot' for more information." << endl;

    return str.str();
}

/****************************************************************************/

void CommandGraph::execute(const StringVector &args)
{
    ec_ioctl_master_t master;
    unsigned int i;
    typedef vector<ec_ioctl_slave_t> SlaveVector;
    SlaveVector slaves;
    ec_ioctl_slave_t slave;
    SlaveVector::const_iterator si;
    map<int, string> portMedia;
    map<int, string>::const_iterator mi;
    map<int, int> mediaWeights;
    map<int, int>::const_iterator wi;

    portMedia[EC_PORT_MII] = "MII";
    mediaWeights[EC_PORT_MII] = 1;

    portMedia[EC_PORT_EBUS] = "EBUS";
    mediaWeights[EC_PORT_EBUS] = 5;
    
    if (args.size()) {
        stringstream err;
        err << "'" << getName() << "' takes no arguments!";
        throwInvalidUsageException(err);
    }

    MasterDevice m(getSingleMasterIndex());
    m.open(MasterDevice::Read);
    m.getMaster(&master);

    for (i = 0; i < master.slave_count; i++) {
        m.getSlave(&slave, i);
        slaves.push_back(slave);
    }

    cout << "/* EtherCAT bus graph. Generated by 'ethercat graph'. */" << endl
        << endl
        << "strict graph bus {" << endl
        << "    rankdir=\"LR\"" << endl
        << "    ranksep=0.8" << endl
        << "    nodesep=0.8" << endl
        << "    node [fontname=\"Helvetica\"]" << endl
        << "    edge [fontname=\"Helvetica\",fontsize=\"10\"]" << endl
        << endl
        << "    master [label=\"EtherCAT\\nMaster\"]" << endl;

    if (slaves.size()) {
        cout << "    master -- slave0"; 
        mi = portMedia.find(slaves.front().ports[0].desc);
        if (mi != portMedia.end())
            cout << "[label=\"" << mi->second << "\"]";

        cout << endl;
    }
    cout << endl;

    for (si = slaves.begin(); si != slaves.end(); si++) {
        cout << "    slave" << si->position << " [shape=\"box\""
            << ",label=\"" << si->position;
        if (string(si->order).size())
            cout << "\\n" << si->order;
        if (si->dc_supported) {
            cout << "\\nDC: ";
            if (si->has_dc_system_time) {
                switch (si->dc_range) {
                    case EC_DC_32:
                        cout << "32 bit";
                        break;
                    case EC_DC_64:
                        cout << "64 bit";
                        break;
                    default:
                        break;
                }
            } else {
                cout << "Delay meas.";
            }
            cout << "\\nDelay: " << si->transmission_delay << " ns";
        }
        cout << "\"]" << endl;

        for (i = 1; i < EC_MAX_PORTS; i++) {
            uint16_t next_pos = si->ports[i].next_slave;
            ec_ioctl_slave_t *next = NULL;

            if (next_pos == 0xffff)
                continue;

            if (next_pos < slaves.size()) {
                next = &slaves[next_pos];
            } else {
                cerr << "Invalid next slave pointer." << endl;
            }

            cout << "    slave" << si->position << " -- "
                << "slave" << next_pos << " [taillabel=\"" << i;

            if (si->dc_supported) {
                cout << " [" << si->ports[i].delay_to_next_dc << "]";
            }
            cout << "\",headlabel=\"0";

            if (next && next->dc_supported) {
                cout << " [" << next->ports[0].delay_to_next_dc << "]";
            }
            cout << "\"";

            mi = portMedia.find(si->ports[i].desc);
            if (mi == portMedia.end() && next) {
                /* Try medium of next-hop slave. */
                mi = portMedia.find(next->ports[0].desc);
            }
            
            if (mi != portMedia.end())
                cout << ",label=\"" << mi->second << "\"";

            wi = mediaWeights.find(si->ports[i].desc);
            if (wi != mediaWeights.end())
                cout << ",weight=\"" << wi->second << "\"";
            
            cout << "]" << endl;
        }

        cout << endl;
    }

    cout << "}" << endl;
}

/*****************************************************************************/
