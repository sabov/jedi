#include "wii.hh"

using namespace std;

//initialize static variables
glm::vec3 Wii::movement = { 0, 0, 0 };
glm::vec3 Wii::acceleration = { 0, 0, 0 };
float Wii::yaw = 0;
float Wii::roll = 0;
float Wii::pitch = 0;
int Wii::LED_MAP[4] = { CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4 };
bool Wii::reloadWiimotes = false;
CWii Wii::wii;
CWiimote* Wii::wiimote = NULL;

Wii::Wii(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world) :
        Input(window, simpleRiftController, world) {

    //Find the wiimote. Search for up to five seconds.
    cout << "Searching for wiimote... Turn it on!" << endl;
    int numFound = wii.Find(5);
    cout << "Found " << numFound << " wiimote(s)" << endl;

    if (numFound == 0) {
        return;
    }

    // Connect to the wiimote
    cout << "Connecting to wiimote..." << endl;

    std::vector<CWiimote>& wiimotes = wii.Connect();
    if (wiimotes.size() == 0) {
        cout << "No wiimote found!" << endl;
        wiimote = NULL;
        return;
    }

    wiimote = &wiimotes[0];
    cout << "Connected to wiimote" << endl;
    wiimote->SetLEDs(LED_MAP[0]);

    // Rumble for 0.2 seconds as a connection acknowledgment
    wiimote->SetRumbleMode(CWiimote::ON);
    usleep(200000);
    wiimote->SetRumbleMode(CWiimote::OFF);

    wiimote->SetMotionSensingMode(CWiimote::ON);
    wiimote->IR.SetMode(CIR::ON);
}

Wii::~Wii() {
    if (wiimote != NULL) {
        //TODO Throws segfault
        //delete wiimote;
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
                cout << "Disconnected wiimote" << endl;
                reloadWiimotes = true;
                break;

            case CWiimote::EVENT_READ_DATA:
                cout << "Data read" << endl;
                break;

            case CWiimote::EVENT_MOTION_PLUS_INSERTED:
                cout << "Motion Plus inserted" << endl;
                wiimote->EnableMotionPlus(CWiimote::ON);
                break;

            case CWiimote::EVENT_MOTION_PLUS_REMOVED:
                cout << "Motion Plus removed" << endl;
                wiimote->EnableMotionPlus(CWiimote::OFF);
                break;

            case CWiimote::EVENT_NUNCHUK_INSERTED:
                cout << "Nunchuk inserted" << endl;
                reloadWiimotes = true;
                break;

            case CWiimote::EVENT_CLASSIC_CTRL_INSERTED:
                cout << "Classic controller inserted" << endl;
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
    // Move lightsaber in space
    //Input::world->moveLightsaber(movement);
    //movement = {0, 0, 0};

    //rotate lightsaber
    Input::world->setRotationLightsaber(ACGL::Math::Functions::calcDegToRad(roll),
            ACGL::Math::Functions::calcDegToRad(270 - pitch), ACGL::Math::Functions::calcDegToRad(yaw));

    //----
    //VIEW
    //----
    // LookAround functionality
    //Input::gSimpleRiftControllerInput->getCamera()->FPSstyleLookAround(movementScale.x, movementScale.y);
}

void Wii::handleEvent(CWiimote &wm) {
    //Movement
    if (wm.Buttons.isPressed(CButtons::BUTTON_UP)) {
        forwardPressed = true;
    } else if (wm.Buttons.isReleased(CButtons::BUTTON_UP)) {
        forwardPressed = false;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_DOWN)) {
        backwardPressed = true;
    } else if (wm.Buttons.isReleased(CButtons::BUTTON_DOWN)) {
        backwardPressed = false;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_LEFT)) {
        leftPressed = true;
    } else if (wm.Buttons.isReleased(CButtons::BUTTON_LEFT)) {
        leftPressed = false;
    }

    if (wm.Buttons.isPressed(CButtons::BUTTON_RIGHT)) {
        rightPressed = true;
    } else if (wm.Buttons.isReleased(CButtons::BUTTON_RIGHT)) {
        rightPressed = false;
    }

    //Force
    if (wm.Buttons.isPressed(CButtons::BUTTON_B)) {
        forcePressed = true;
        wiimote->SetRumbleMode(CWiimote::ON);
    } else if (wm.Buttons.isReleased(CButtons::BUTTON_B)) {
        forcePressed = false;
        wiimote->SetRumbleMode(CWiimote::OFF);
    }

    // if the accelerometer is turned on then print angles
    if (wm.isUsingACC()) {
        float x, dx, y, dy, z, dz;
        float pitchNew, rollNew, yawNew;
        wm.Accelerometer.GetOrientation(pitchNew, rollNew, yawNew);

        if (glm::abs(pitchNew - pitch) > 5) {
            pitch = pitchNew;
            cout << "pitch " << pitch << endl;
        }

        if (glm::abs(rollNew - roll) > 5) {
            roll = rollNew;
            cout << "roll " << roll << endl;
        }

        if (glm::abs(yawNew - yaw) > 5) {
            yaw = yawNew;
            cout << "yaw " << yaw << endl;
        }

        wm.Accelerometer.GetGravityVector(x, y, z);
        dx = acceleration.x - x;
        if (glm::abs(dx) > 0.1) {
            cout << "X: " << dx << endl;
            movement.x += dx;
        }
        acceleration.x = x;
        dy = acceleration.y - y;
        if (glm::abs(dy) > 0.1) {
            cout << "Y: " << dy << endl;
            movement.y += dy;
        }
        acceleration.y = y;
        dz = acceleration.z - z;
        if (glm::abs(dz) > 0.1) {
            cout << "Z: " << dz << endl;
            movement.z += dz;
        }
        acceleration.z = z;
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
    /*if (wm.isUsingIR()) {
     std::vector<CIRDot>::iterator i;
     int x, y;
     int index;

     cout << "Num IR Dots: " << wm.IR.GetNumDots() << endl;
     cout << "IR State: " << wm.IR.GetState() << endl;

     std::vector<CIRDot>& dots = wm.IR.GetDots();

     for (index = 0, i = dots.begin(); i != dots.end(); ++index, ++i) {
     if ((*i).isVisible()) {
     (*i).GetCoordinate(x, y);
     cout << "IR source " << index << ": (" << x << ", " << y << ")" << endl;

     wm.IR.GetCursorPosition(x, y);
     cout << "IR cursor: (" << x << ", " << y << ")" << endl;
     cout << "IR z distance: " << wm.IR.GetDistance() << endl;
     }
     }
     }*/

    if (wm.ExpansionDevice.GetType() == wm.ExpansionDevice.TYPE_NUNCHUK) {
        float pitch, roll, yaw, a_pitch, a_roll;
        float angle, magnitude;

        CNunchuk &nc = wm.ExpansionDevice.Nunchuk;

        if (nc.Buttons.isPressed(CNunchukButtons::BUTTON_C)) {
            cout << "Nunchuk C pressed" << endl;
        }

        if (nc.Buttons.isPressed(CNunchukButtons::BUTTON_Z)) {
            cout << "Nunchuk Z pressed" << endl;
        }

        nc.Accelerometer.GetOrientation(pitch, roll, yaw);
        nc.Accelerometer.GetRawOrientation(a_pitch, a_roll);
        cout << "nunchuk roll = " << roll << "[" << a_roll << "]" << endl;
        cout << "nunchuk pitch = " << pitch << "[" << a_pitch << "]" << endl;
        cout << "nunchuk yaw = " << yaw << endl;

        nc.Joystick.GetPosition(angle, magnitude);
        cout << "Nunchuk joystick angle = " << angle << endl;
        cout << "Nunchuk joystick magnitude = " << magnitude << endl;
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
