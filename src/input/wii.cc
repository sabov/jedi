#include "wii.hh"

using namespace std;

//initialize static variables
glm::dvec2 Wii::initialPosition;
glm::dvec2 Wii::movement;
glm::dvec2 Wii::movementScale;
bool Wii::rightMouseButtonDown = false;
bool Wii::leftMouseButtonDown = false;
bool Wii::mouseWheelScrollUp = false;
bool Wii::mouseWheelScrollDown = false;

Wii::Wii(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world) :
        Input(window, simpleRiftController, world) {
    // handle the mouse as a callback
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // handle the mouse wheel as a callback
    glfwSetScrollCallback(window, mouseWheelCallback);
}

// Whenever a mouse button gets pushed and the mouse dragged the cursor should get
// hidden. To prevent the mouse to leave the window, the cursor will get reset to the
// center after each run of this function, to not confuse the user it will get
// moved to the original position when the button gets released.
// Enables FPS View while holding the right mouse button
void Wii::mouseMoveCallback(GLFWwindow *window, double x, double y) {
    // Get the window size (do this each mouse press in case the window was resized,
    // this could be stored and only get refreshed in case a resize happened but this
    // would need more global state -> let's keep this example simple).
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    windowSize = glm::dvec2(width, height);

    // Get the movement in pixels.
    // Y axis up should be positive. A scaling of the mouse speed can be done here as well!
    movement = glm::dvec2(x, y) - 0.5 * windowSize;
    movement *= glm::dvec2(1.0, -1.0);
    movementScale.x = movement.x / windowSize.x;
    movementScale.y = -movement.y / windowSize.y;

    bool leftMouseButtonPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
    bool rightMouseButtonPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);

    if ((!rightMouseButtonDown && rightMouseButtonPressed) || (!leftMouseButtonDown && leftMouseButtonPressed)) {
        //New button pressed
        initialPosition = glm::dvec2(x, y);

        // no movement in this call:
        x = width * 0.5;
        y = height * 0.5;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide the cursor

        if (!rightMouseButtonDown && rightMouseButtonPressed) {
            rightMouseButtonDown = true;
        }
        if (!leftMouseButtonDown && leftMouseButtonPressed) {
            leftMouseButtonDown = true;
        }

    } else if ((rightMouseButtonDown && !rightMouseButtonPressed) || (leftMouseButtonDown && !leftMouseButtonPressed)) {
        //New button not pressed
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // unhide the cursor
        glfwSetCursorPos(window, initialPosition.x, initialPosition.y);

        if (rightMouseButtonDown && !rightMouseButtonPressed) {
            rightMouseButtonDown = false;
        }
        if (leftMouseButtonDown && !leftMouseButtonPressed) {
            leftMouseButtonDown = false;
        }
    }

    if (rightMouseButtonDown || leftMouseButtonDown) {
        //Reset current cursor position
        glfwSetCursorPos(window, windowSize.x * 0.5, windowSize.y * 0.5);
    }
}

//Handles the mouse wheel
void Wii::mouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        mouseWheelScrollUp = true;
    } else {
        mouseWheelScrollDown = true;
    }
}

void Wii::handleInput() {
    Input::handleInput();

    //----------
    //LIGHTSABER
    //----------
    //Move in space
    if (leftMouseButtonDown) {
        // Move lightsaber
        glm::vec3 lightsaberMovement;
        lightsaberMovement.x = movementScale.x;
        if (controlPressed) {
            //Move on z-axis instead of y-axis
            lightsaberMovement.z = movementScale.y;
        } else {
            lightsaberMovement.y = -movementScale.y;
        }
        Input::world->moveLightsaber(lightsaberMovement);
    }

    //rotate
    float rotateRad = ACGL::Math::Functions::calcDegToRad(5.0f);
    if (mouseWheelScrollUp) {
        if (controlPressed) {
            Input::world->rotateLightsaber(0.0f, -rotateRad, 0.0f);
        } else {
            Input::world->rotateLightsaber(0.0f, 0.0f, -rotateRad);
        }
        //Reset state
        mouseWheelScrollUp = false;
    } else if (mouseWheelScrollDown) {
        if (controlPressed) {
            Input::world->rotateLightsaber(0.0f, rotateRad, 0.0f);
        } else {
            Input::world->rotateLightsaber(0.0f, 0.0f, rotateRad);
        }
        //Reset state
        mouseWheelScrollDown = false;
    }

    //----
    //VIEW
    //----
    if (rightMouseButtonDown) {
        // LookAround functionality
        Input::gSimpleRiftControllerInput->getCamera()->FPSstyleLookAround(movementScale.x, movementScale.y);
    }
}

/* #######################
 * CODE FROM EXAMPLE
 * #######################
 */
int Wii::LED_MAP[4] = {CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4};

void Wii::handleEvent(CWiimote &wm) {
    char prefixString[64];

    if (wm.Buttons.isJustPressed(CButtons::BUTTON_MINUS)) {
        wm.SetMotionSensingMode(CWiimote::OFF);
    }

    if (wm.Buttons.isJustPressed(CButtons::BUTTON_PLUS)) {
        wm.SetMotionSensingMode(CWiimote::ON);
    }

    if (wm.Buttons.isJustPressed(CButtons::BUTTON_DOWN)) {
        wm.IR.SetMode(CIR::OFF);
    }

    if (wm.Buttons.isJustPressed(CButtons::BUTTON_UP)) {
        wm.IR.SetMode(CIR::ON);
    }

    if (wm.Buttons.isJustPressed(CButtons::BUTTON_RIGHT)) {
        wm.EnableMotionPlus(CWiimote::ON);
    }

    if (wm.Buttons.isJustPressed(CButtons::BUTTON_LEFT)) {
        wm.EnableMotionPlus(CWiimote::OFF);
    }

    if (wm.Buttons.isJustPressed(CButtons::BUTTON_B)) {
        wm.ToggleRumble();
    }

    sprintf(prefixString, "Controller [%i]: ", wm.GetID());

    if (wm.Buttons.isPressed(CButtons::BUTTON_A)) {
        printf("%s A pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_B)) {
        printf("%s B pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_UP)) {
        printf("%s Up pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_DOWN)) {
        printf("%s Down pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_LEFT)) {
        printf("%s Left pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_RIGHT)) {
        printf("%s Right pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_MINUS)) {
        printf("%s Minus pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_PLUS)) {
        printf("%s Plus pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_ONE)) {
        printf("%s One pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_TWO)) {
        printf("%s Two pressed\n", prefixString);
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_HOME)) {
        printf("%s Home pressed\n", prefixString);
    }

    // if the accelerometer is turned on then print angles
    if (wm.isUsingACC()) {
        float pitch, roll, yaw, a_pitch, a_roll;
        wm.Accelerometer.GetOrientation(pitch, roll, yaw);
        wm.Accelerometer.GetRawOrientation(a_pitch, a_roll);
        printf("%s wiimote roll = %f [%f]\n", prefixString, roll, a_roll);
        printf("%s wiimote pitch = %f [%f]\n", prefixString, pitch, a_pitch);
        printf("%s wiimote yaw = %f\n", prefixString, yaw);
    }

    // if the Motion Plus is turned on then print angles
    if (wm.isUsingMotionPlus()) {
        float roll_rate, pitch_rate, yaw_rate;
        wm.ExpansionDevice.MotionPlus.Gyroscope.GetRates(roll_rate, pitch_rate, yaw_rate);

        printf("%s motion plus roll rate = %f\n", prefixString, roll_rate);
        printf("%s motion plus pitch rate = %f\n", prefixString, pitch_rate);
        printf("%s motion plus yaw rate = %f\n", prefixString, yaw_rate);
    }

    // if(IR tracking is on then print the coordinates
    if (wm.isUsingIR()) {
        std::vector<CIRDot>::iterator i;
        int x, y;
        int index;

        printf("%s Num IR Dots: %i\n", prefixString, wm.IR.GetNumDots());
        printf("%s IR State: %u\n", prefixString, wm.IR.GetState());

        std::vector<CIRDot>& dots = wm.IR.GetDots();

        for (index = 0, i = dots.begin(); i != dots.end(); ++index, ++i) {
            if ((*i).isVisible()) {
                (*i).GetCoordinate(x, y);
                printf("%s IR source %i: (%i, %i)\n", prefixString, index, x, y);

                wm.IR.GetCursorPosition(x, y);
                printf("%s IR cursor: (%i, %i)\n", prefixString, x, y);
                printf("%s IR z distance: %f\n", prefixString, wm.IR.GetDistance());
            }
        }
    }

    int exType = wm.ExpansionDevice.GetType();
    if (exType == wm.ExpansionDevice.TYPE_NUNCHUK) {
        float pitch, roll, yaw, a_pitch, a_roll;
        float angle, magnitude;

        CNunchuk &nc = wm.ExpansionDevice.Nunchuk;

        sprintf(prefixString, "Nunchuk [%i]: ", wm.GetID());

        if (nc.Buttons.isPressed(CNunchukButtons::BUTTON_C)) {
            printf("%s C pressed\n", prefixString);
        }

        if (nc.Buttons.isPressed(CNunchukButtons::BUTTON_Z)) {
            printf("%s Z pressed\n", prefixString);
        }

        nc.Accelerometer.GetOrientation(pitch, roll, yaw);
        nc.Accelerometer.GetRawOrientation(a_pitch, a_roll);
        printf("%s roll = %f [%f]\n", prefixString, roll, a_roll);
        printf("%s pitch = %f [%f]\n", prefixString, pitch, a_pitch);
        printf("%s yaw = %f\n", prefixString, yaw);

        nc.Joystick.GetPosition(angle, magnitude);
        printf("%s joystick angle = %f\n", prefixString, angle);
        printf("%s joystick magnitude = %f\n", prefixString, magnitude);
    } else if (exType == wm.ExpansionDevice.TYPE_CLASSIC) {
        float angle, magnitude;

        CClassic &cc = wm.ExpansionDevice.Classic;

        sprintf(prefixString, "Classic [%i]: ", wm.GetID());

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_A)) {
            printf("%s A pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_B)) {
            printf("%s B pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_X)) {
            printf("%s X pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_Y)) {
            printf("%s Y pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_LEFT)) {
            printf("%s Left pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_UP)) {
            printf("%s Up pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_RIGHT)) {
            printf("%s Right pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_DOWN)) {
            printf("%s Down pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_PLUS)) {
            printf("%s Plus pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_MINUS)) {
            printf("%s Minus pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_HOME)) {
            printf("%s Home pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_ZL)) {
            printf("%s ZL pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_FULL_L)) {
            printf("%s ZR pressed\n", prefixString);
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_FULL_R)) {
            printf("%s ZR pressed\n", prefixString);
        }

        printf("%s L button pressed = %f\n", prefixString, cc.GetLShoulderButton());
        printf("%s R button pressed = %f\n", prefixString, cc.GetRShoulderButton());

        cc.LeftJoystick.GetPosition(angle, magnitude);
        printf("%s left joystick angle = %f\n", prefixString, angle);
        printf("%s left joystick magnitude = %f\n", prefixString, magnitude);

        cc.RightJoystick.GetPosition(angle, magnitude);
        printf("%s right joystick angle = %f\n", prefixString, angle);
        printf("%s right joystick magnitude = %f\n", prefixString, magnitude);
    } else if (exType == wm.ExpansionDevice.TYPE_GUITAR_HERO_3) {
        float angle, magnitude;

        CGuitarHero3 &gh = wm.ExpansionDevice.GuitarHero3;

        sprintf(prefixString, "Guitar [%i]: ", wm.GetID());

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_STRUM_UP)) {
            printf("%s Strum Up pressed\n", prefixString);
        }

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_STRUM_DOWN)) {
            printf("%s Strum Down pressed\n", prefixString);
        }

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_YELLOW)) {
            printf("%s Yellow pressed\n", prefixString);
        }

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_GREEN)) {
            printf("%s Green pressed\n", prefixString);
        }

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_BLUE)) {
            printf("%s Blue pressed\n", prefixString);
        }

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_RED)) {
            printf("%s Red pressed\n", prefixString);
        }

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_ORANGE)) {
            printf("%s Orange pressed\n", prefixString);
        }

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_PLUS)) {
            printf("%s Plus pressed\n", prefixString);
        }

        if (gh.Buttons.isPressed(CGH3Buttons::BUTTON_MINUS)) {
            printf("%s Minus pressed\n", prefixString);
        }

        printf("%s whammy bar = %f\n", prefixString, gh.GetWhammyBar());

        gh.Joystick.GetPosition(angle, magnitude);
        printf("%s joystick angle = %f\n", prefixString, angle);
        printf("%s joystick magnitude = %f\n", prefixString, magnitude);
    } else if (exType == wm.ExpansionDevice.TYPE_BALANCE_BOARD) {
        CBalanceBoard &bb = wm.ExpansionDevice.BalanceBoard;
        float total, topLeft, topRight, bottomLeft, bottomRight;

        bb.WeightSensor.GetWeight(total, topLeft, topRight, bottomLeft, bottomRight);
        printf("balance board top left weight: %f\n", topLeft);
        printf("balance board top right weight: %f\n", topRight);
        printf("balance board bottom left weight: %f\n", bottomLeft);
        printf("balance board bottom right weight: %f\n", bottomRight);
        printf("balance board total weight: %f\n", total);
    }
}

void Wii::handleStatus(CWiimote &wm) {
    printf("\n");
    printf("--- CONTROLLER STATUS [wiimote id %i] ---\n\n", wm.GetID());

    printf("attachment: %i\n", wm.ExpansionDevice.GetType());
    printf("speaker: %i\n", wm.isUsingSpeaker());
    printf("ir: %i\n", wm.isUsingIR());
    printf("leds: %i %i %i %i\n", wm.isLEDSet(1), wm.isLEDSet(2), wm.isLEDSet(3), wm.isLEDSet(4));
    printf("battery: %f %%\n", wm.GetBatteryLevel());
}

void Wii::handleDisconnect(CWiimote &wm) {
    printf("\n");
    printf("--- DISCONNECTED [wiimote id %i] ---\n", wm.GetID());
    printf("\n");
}

void Wii::handleReadData(CWiimote &wm) {
    printf("\n");
    printf("--- DATA READ [wiimote id %i] ---\n", wm.GetID());
    printf("\n");
}

void Wii::handleNunchukInserted(CWiimote &wm) {
    printf("Nunchuk inserted on controller %i.\n", wm.GetID());
}

void Wii::handleClassicInserted(CWiimote &wm) {
    printf("Classic controler inserted on controller %i.\n", wm.GetID());
}

void Wii::handleGH3Inserted(CWiimote &wm) {
    printf("GH3 inserted on controller %i.\n", wm.GetID());
}

void Wii::main() {
    CWii wii; // Defaults to 4 remotes
    std::vector<CWiimote>::iterator i;
    int reloadWiimotes = 0;
    int numFound;
    int index;

    cout << "Searching for wiimotes... Turn them on!" << endl;

    //Find the wiimote
    numFound = wii.Find(5);

    // Search for up to five seconds;

    cout << "Found " << numFound << " wiimotes" << endl;
    cout << "Connecting to wiimotes..." << endl;

    // Connect to the wiimote
    std::vector<CWiimote>& wiimotes = wii.Connect();

    cout << "Connected to " << (unsigned int) wiimotes.size() << " wiimotes" << endl;

    // Setup the wiimotes
    for (index = 0, i = wiimotes.begin(); i != wiimotes.end(); ++i, ++index) {
        // Use a reference to make working with the iterator handy.
        CWiimote & wiimote = *i;

        //Set Leds
        wiimote.SetLEDs(LED_MAP[index]);

        //Rumble for 0.2 seconds as a connection ack
        wiimote.SetRumbleMode(CWiimote::ON);
        usleep(200000);
        wiimote.SetRumbleMode(CWiimote::OFF);
    }

    cout << "\nPress PLUS (MINUS) to enable (disable) Motion Sensing Report (only accelerometers)" << endl;
    cout << "Press RIGHT (LEFT) to enable (disable) Motion Plus (requires Motion Sensing enabled)" << endl;
    cout << "Press UP (DOWN) to enable (disable) IR camera (requires some IR led)" << endl;

    do {
        if (reloadWiimotes) {
            // Regenerate the list of wiimotes
            wiimotes = wii.GetWiimotes();
            reloadWiimotes = 0;
        }

        //Poll the wiimotes to get the status like pitch or roll
        if (wii.Poll()) {
            for (i = wiimotes.begin(); i != wiimotes.end(); ++i) {
                // Use a reference to make working with the iterator handy.
                CWiimote & wiimote = *i;
                switch (wiimote.GetEvent()) {

                case CWiimote::EVENT_EVENT:
                    handleEvent(wiimote);
                    break;

                case CWiimote::EVENT_STATUS:
                    handleStatus(wiimote);
                    break;

                case CWiimote::EVENT_DISCONNECT:
                case CWiimote::EVENT_UNEXPECTED_DISCONNECT:
                    handleDisconnect(wiimote);
                    reloadWiimotes = 1;
                    break;

                case CWiimote::EVENT_READ_DATA:
                    handleReadData(wiimote);
                    break;

                case CWiimote::EVENT_NUNCHUK_INSERTED:
                    handleNunchukInserted(wiimote);
                    reloadWiimotes = 1;
                    break;

                case CWiimote::EVENT_CLASSIC_CTRL_INSERTED:
                    handleClassicInserted(wiimote);
                    reloadWiimotes = 1;
                    break;

                case CWiimote::EVENT_GUITAR_HERO_3_CTRL_INSERTED:
                    handleGH3Inserted(wiimote);
                    reloadWiimotes = 1;
                    break;

                case CWiimote::EVENT_MOTION_PLUS_INSERTED:
                    cout << "Motion Plus inserted." << endl;
                    break;

                case CWiimote::EVENT_BALANCE_BOARD_INSERTED:
                    cout << "Balance Board connected.\n" << endl;
                    break;

                case CWiimote::EVENT_BALANCE_BOARD_REMOVED:
                    cout << "Balance Board disconnected.\n" << endl;
                    break;

                case CWiimote::EVENT_NUNCHUK_REMOVED:
                case CWiimote::EVENT_CLASSIC_CTRL_REMOVED:
                case CWiimote::EVENT_GUITAR_HERO_3_CTRL_REMOVED:
                case CWiimote::EVENT_MOTION_PLUS_REMOVED:
                    cout << "An expansion was removed." << endl;
                    handleStatus(wiimote);
                    reloadWiimotes = 1;
                    break;

                default:
                    break;
                }
            }
        }

    } while (wiimotes.size()); // Go so long as there are wiimotes left to poll
}
