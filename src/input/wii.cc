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
int Wii::LED_MAP[4] = { CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4 };
bool Wii::reloadWiimotes = false;
CWii Wii::wii;
CWiimote* Wii::wiimote = NULL;

Wii::Wii(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world) :
        Input(window, simpleRiftController, world) {
    // handle the mouse as a callback
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // handle the mouse wheel as a callback
    glfwSetScrollCallback(window, mouseWheelCallback);

    init();
}

Wii::~Wii() {
    if (wiimote != NULL) {
        //TODO Throws segfault
        //delete wiimote;
    }
}

//Search for wiimote and initialize it
void Wii::init() {
    //Find the wiimote. Search for up to five seconds.
    cout << "Searching for wiimote... Turn them on!" << endl;
    int numFound = wii.Find(5);
    cout << "Found " << numFound << " wiimote(s)" << endl;

    if (numFound == 0) {
        return;
    }

    // Connect to the wiimote
    cout << "Connecting to wiimote..." << endl;

    std::vector<CWiimote>& wiimotes = wii.Connect();
    if (wiimotes.size() > 0) {
        wiimote = &wiimotes[0];
        cout << "Connected to wiimote" << endl;

        // Set LEDs
        wiimote->SetLEDs(LED_MAP[0]);

        // Rumble for 0.2 seconds as a connection acknowledgment
        wiimote->SetRumbleMode(CWiimote::ON);
        usleep(200000);
        wiimote->SetRumbleMode(CWiimote::OFF);
    } else {
        wiimote = NULL;
    }
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
    // TODO Only get ontime result of polling when doing it twice in one iteration. I don't now why.
    for (int i = 0; i < 2; i++) {
        if (wii.Poll()) {
            if (reloadWiimotes) {
                // Regenerate the list of wiimotes
                cout << "Reload wiimotes" << endl;
                std::vector<CWiimote>& wiimotes = wii.GetWiimotes();
                if (wiimotes.size() > 0) {
                    wiimote = &wiimotes[0];
                } else {
                    wiimote = NULL;
                    return;
                }
            }

            switch (wiimote->GetEvent()) {

            case CWiimote::EVENT_EVENT:
                handleEvent(*wiimote);
                break;

            case CWiimote::EVENT_STATUS:
                handleStatus(*wiimote);
                break;

            case CWiimote::EVENT_DISCONNECT:
            case CWiimote::EVENT_UNEXPECTED_DISCONNECT:
                handleDisconnect(*wiimote);
                reloadWiimotes = true;
                break;

            case CWiimote::EVENT_READ_DATA:
                handleReadData(*wiimote);
                break;

            case CWiimote::EVENT_NUNCHUK_INSERTED:
                handleNunchukInserted(*wiimote);
                reloadWiimotes = true;
                break;

            case CWiimote::EVENT_CLASSIC_CTRL_INSERTED:
                handleClassicInserted(*wiimote);
                reloadWiimotes = true;
                break;

            case CWiimote::EVENT_MOTION_PLUS_INSERTED:
                cout << "Motion Plus inserted." << endl;
                break;

            case CWiimote::EVENT_NUNCHUK_REMOVED:
            case CWiimote::EVENT_CLASSIC_CTRL_REMOVED:
                cout << "An expansion was removed." << endl;
                handleStatus(*wiimote);
                reloadWiimotes = true;
                break;

            default:
                break;
            }
        }
    }

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

    if (wm.Buttons.isPressed(CButtons::BUTTON_A)) {
        cout << "A pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_B)) {
        cout << "B pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_UP)) {
        forwardPressed = true;
    } else if (wm.Buttons.isReleased(CButtons::BUTTON_UP)) {
        forwardPressed = false;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_DOWN)) {
        cout << "Down pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_LEFT)) {
        cout << "Left pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_RIGHT)) {
        cout << "Right pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_MINUS)) {
        cout << "Minus pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_PLUS)) {
        cout << "Plus pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_ONE)) {
        cout << "One pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_TWO)) {
        cout << "Two pressed" << endl;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_HOME)) {
        cout << "Home pressed" << endl;
    }

    // if the accelerometer is turned on then print angles
    if (wm.isUsingACC()) {
        float pitch, roll, yaw, a_pitch, a_roll;
        wm.Accelerometer.GetOrientation(pitch, roll, yaw);
        wm.Accelerometer.GetRawOrientation(a_pitch, a_roll);
        cout << "wiimote roll = " << roll << "[" << a_roll << "]" << endl;
        cout << "wiimote pitch = " << pitch << "[" << a_pitch << "]" << endl;
        cout << "wiimote yaw = " << yaw << endl;
    }

    // if the Motion Plus is turned on then print angles
    if (wm.isUsingMotionPlus()) {
        float roll_rate, pitch_rate, yaw_rate;
        wm.ExpansionDevice.MotionPlus.Gyroscope.GetRates(roll_rate, pitch_rate, yaw_rate);

        cout << "motion plus roll rate = " << roll_rate << endl;
        cout << "motion plus pitch rate = " << pitch_rate << endl;
        cout << "motion plus yaw rate = " << yaw_rate << endl;
    }

    // if IR tracking is on then print the coordinates
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
        cout << "nunchuk roll = " << roll << "[" << a_roll << "]" << endl;
        cout << "nunchuk pitch = " << pitch << "[" << a_pitch << "]" << endl;
        cout << "nunchuk yaw = " << yaw << endl;

        nc.Joystick.GetPosition(angle, magnitude);
        printf("%s joystick angle = %f\n", prefixString, angle);
        printf("%s joystick magnitude = %f\n", prefixString, magnitude);
    } else if (exType == wm.ExpansionDevice.TYPE_CLASSIC) {
        float angle, magnitude;

        CClassic &cc = wm.ExpansionDevice.Classic;

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_A)) {
            cout << "Classic: A pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_B)) {
            cout << "Classic: B pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_X)) {
            cout << "Classic: X pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_Y)) {
            cout << "Classic: Y pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_LEFT)) {
            cout << "Classic: Left pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_UP)) {
            cout << "Classic: Up pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_RIGHT)) {
            cout << "Classic: Right pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_DOWN)) {
            cout << "Classic: Down pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_PLUS)) {
            cout << "Classic: Plus pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_MINUS)) {
            cout << "Classic: Minus pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_HOME)) {
            cout << "Classic: Home pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_ZL)) {
            cout << "Classic: ZL pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_FULL_L)) {
            cout << "Classic: ZR pressed" << endl;
        }

        if (cc.Buttons.isPressed(CClassicButtons::BUTTON_FULL_R)) {
            cout << "Classic: ZR pressed" << endl;
        }

        printf("%s L button pressed = %f\n", prefixString, cc.GetLShoulderButton());
        printf("%s R button pressed = %f\n", prefixString, cc.GetRShoulderButton());

        cc.LeftJoystick.GetPosition(angle, magnitude);
        printf("%s left joystick angle = %f\n", prefixString, angle);
        printf("%s left joystick magnitude = %f\n", prefixString, magnitude);

        cc.RightJoystick.GetPosition(angle, magnitude);
        printf("%s right joystick angle = %f\n", prefixString, angle);
        printf("%s right joystick magnitude = %f\n", prefixString, magnitude);
    }
}

void Wii::handleStatus(CWiimote &wm) {
    cout << "Controller status" << endl;
    cout << "attachment: " << wm.ExpansionDevice.GetType() << endl;
    cout << "speaker: " << wm.isUsingSpeaker() << endl;
    cout << "ir: " << wm.isUsingIR() << endl;
    cout << "leds: " << wm.isLEDSet(1) << wm.isLEDSet(2) << wm.isLEDSet(3) << wm.isLEDSet(4) << endl;
    cout << "battery: " << wm.GetBatteryLevel() << endl;
}

void Wii::handleDisconnect(CWiimote &wm) {
    cout << "Disconnected wiimote" << endl;
}

void Wii::handleReadData(CWiimote &wm) {
    cout << "Data read" << endl;
}

void Wii::handleNunchukInserted(CWiimote &wm) {
    cout << "Nunchuk inserted" << endl;
}

void Wii::handleClassicInserted(CWiimote &wm) {
    cout << "Classic controller inserted" << endl;
}
