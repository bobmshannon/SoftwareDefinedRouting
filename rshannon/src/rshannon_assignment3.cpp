/**
 * @ubitname_assignment3
 * @author  Fullname <ubitname@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../include/tcp_server.h"
#include "../include/controller.h"
#include "../include/router.h"
#include "../include/data.h"
#include "../include/error.h"

using namespace std;

void show_usage(string progname) {
    cerr << "Usage: " + progname + " <CONTROL PORT>" << endl
         << "    <CONTROL PORT> [0-65535]" << endl
         << "    The port to listen for control messages on."
         << endl;
}

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
    // Argument checking
    if (argc != 2) {
        show_usage(string(argv[0]));
        return -1;
    }

    // Get arguments
    string control_port(argv[1]);

 	// More argument checking
 	if (atoi(control_port.c_str()) > 65535 || atoi(control_port.c_str()) < 0) {
        show_usage(string(argv[0]));
        return -1;
    }

    // Initialize controller. The controller manages the input received
    // from the control, data, and router ports and generates the appropriate
    // actions. Note that this controller should not be confused with the controller
    // that sends messages to the process through the control port.
    Controller controller = Controller();
    controller.start(control_port);

	return 0;
}
