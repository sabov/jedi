#include "wii.hh"

using namespace std;

//initialize static variables
const float Wii::SMOOTH_FACTOR_GYRO = 0.90;
const float Wii::SMOOTH_FACTOR_ACCEL = 0.20;

glm::vec3 Wii::movement = { 0, 0, 0 };
glm::vec3 Wii::acceleration = { 0, 0, 0 };
float Wii::yaw = 0;
float Wii::roll = 0;
float Wii::pitch = 0;
int Wii::LED_MAP[4] = { CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4 };
bool Wii::reloadWiimotes = false;
CWii Wii::wii;
CWiimote* Wii::wiimote = NULL;

double Wii::timeLast = 0;
double Wii::timePassed = 0;

Wii::Wii(GLFWwindow* window, ACGL::HardwareSupport::SimpleRiftController *simpleRiftController, World *world) :
        Input(window, simpleRiftController, world) {

    //Find the wiimote. Search for up to five seconds.
    message() << "Searching for wiimote... Turn it on!" << endl;
    int numFound = wii.Find(5);//LoadRegisteredWiimotes();
    debug() << "Found " << numFound << " wiimote(s)" << endl;

    if (numFound == 0) {
        return;
    }

    // Connect to the wiimote
    debug() << "Connecting to wiimote..." << endl;

    std::vector<CWiimote>& wiimotes = wii.Connect();
    if (wiimotes.size() == 0) {
        error() << "No wiimote found!" << endl;
        wiimote = NULL;
        return;
    }

    wiimote = &wiimotes[0];
    message() << "Connected to wiimote" << endl;
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
    //Get passed time for movement over time
    double timeNow = glfwGetTime();
    timePassed = timeNow - timeLast;
    timeLast = timeNow;

    // TODO Only get ontime result of polling when doing it twice in one iteration. I don't now why.
    for (int i = 0; i < 10; i++) {
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
    Input::world->moveLightsaber(movement);
    movement = {0, 0, 0};

    //rotate lightsaber
    glm::mat4 rotation = glm::yawPitchRoll(ACGL::Math::Functions::calcDegToRad(yaw),
            ACGL::Math::Functions::calcDegToRad(270 - pitch), ACGL::Math::Functions::calcDegToRad(roll));
    Input::world->setRotationMatrixLightsaber(rotation);

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
        float acc_x, dx, acc_y, dy, acc_z, dz;
        float pitchNew, rollNew, yawNew;
        double timeFactor = timePassed * timePassed * 25;

        // Get data from accelerator and gyroscope
        wm.Accelerometer.GetOrientation(pitchNew, rollNew, yawNew);
        wm.Accelerometer.GetGravityVector(acc_x, acc_z, acc_y); //Wiimote switches y- and z-axis
        acc_y = 1 - acc_y; //Gravity of 1g

        //Low pass for smoothing data
        pitch = fmod(SMOOTH_FACTOR_GYRO * pitch + (1 - SMOOTH_FACTOR_GYRO) * pitchNew, 360);
        roll = fmod(SMOOTH_FACTOR_GYRO * roll + (1 - SMOOTH_FACTOR_GYRO) * rollNew, 360);
        yaw = fmod(SMOOTH_FACTOR_GYRO * yaw + (1 - SMOOTH_FACTOR_GYRO) * yawNew, 360);
        acceleration.x = (SMOOTH_FACTOR_ACCEL * acceleration.x) + (1 - SMOOTH_FACTOR_ACCEL) * acc_x;
        acceleration.y = (SMOOTH_FACTOR_ACCEL * acceleration.y) + (1 - SMOOTH_FACTOR_ACCEL) * acc_y;
        acceleration.z = (SMOOTH_FACTOR_ACCEL * acceleration.z) + (1 - SMOOTH_FACTOR_ACCEL) * acc_z;

        dx = acceleration.x * timeFactor;
        movement.x += dx;
        dy = acceleration.y * timeFactor;
        movement.y += dy;
        dz = acceleration.z * timeFactor;
        movement.z += dz;
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
