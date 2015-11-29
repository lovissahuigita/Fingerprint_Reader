#include "mbed.h"
#include "GT511C3.hpp"
#include "uLCD_4DGL.h"
//#include "SDFileSystem.h"

// LCD, IO
uLCD_4DGL uLCD(p28, p27, p30);
Serial debug(USBTX,USBRX);
GT511C3 finger(p9,p10);
//SDFileSystem sd(p5, p6, p7, p8, "sd");

// Set these variables to 0 after each read
// KEYPAD_BOTTOM LEFT
DigitalOut is_cancelled(LED1);
// KEYPAD_BOTTOM_RIGHT
DigitalOut is_entered(LED2);

// FUNCTIONS:
void user_menu();
void open_existing_system();
void start_session();
void close_session();
void read_from_fingerprint_scanner();
void close_session();
int read_keypad_group_id(char* readable);
void upload_to_scanner();
void download_to_memory();
void send_file_via_wifi();
int owner_database_contains(char* gid);

// CONSTANT VARIABLES
const int id_size = 4;
const int group_size = 200;
const int owner_id_size = 20;
const char* owner_id_file = "owner_id_file.txt";

// VARIABLES
char group_id[id_size]; // group_id null >> no open session
int this_session_id[group_size];


int main() {
    user_menu();
    return 0;
}

void user_menu() {
    
    uLCD.printf("***********************");
    uLCD.printf("* Fingerprint Scanner *");
    uLCD.printf("***********************\n\n");
    uLCD.printf("Welcome!\n\n");
    open_existing_system();
}

void open_existing_system() {
    
    is_done = 0;
    while (!is_done){
        
        //Ask for user input
        uLCD.printf("Enter group ID: ");
            
        // get user input >> read group id ******
        char from_user[id_size];
        read_keypad_group_id(from_user);
        
        // check the input against owner data
        if (owner_database_contains(from_user)) {
            start_session();
        }

        // if the cancelled button is pressed, shutdown
        if (is_cancelled) {
            is_cancelled = 0;
            
            uLCD.printf("Are you sure you want to shut down?\n");
            
            //TODO: DO SOMETHING BETTER
            while (!is_entered && !is_cancelled) {
            }
            
            if (is_entered) {
                is_done = 1;
            }
            reset_buttons();
        }  
    }
}

/*
 * This method checks whether the database contains the specific group
 * that the user specified using the id
 * 
 * @param char* gid user specified string id "****" of length id_size
 * @return int 1 if true, 0 otherwise
 */
int owner_database_contains(char* gid) {
    FILE *owner_id_file;
    owner_file = fopen(owner_id_file,"r");
    if (owner_file) {
        
        char temp[id_size];
        owner_file.getline(temp, id_size);
        while (temp) {
            if (int(*gid) == int(*temp)) {
                fclose(owner_file);
                return 1;
            }
            owner_file.getline(temp, id_size);
        }

    }
    fclose(owner_file);
    return 0;
}

void start_session() {
    // upload the specified database to the fingerprint
    upload_to_scanner();

    // display options to user via LCD
    uLCD.printf("Session <session_name> is open.\n");
    uLCD.printf("Scan fingerprint to enroll!\n\n");
    uLCD.printf("Press <X> to close session\n");
    
    int is_closed = 0;
    while (!is_closed) {
        
        // read the fingerprint
        read_from_fingerprint_scanner();
        
        // read whether is_closed is updated
        if (is_cancelled) {
            reset_buttons();       
                
            // read the keypad for group id and put it in input_group_id
            char input_group_id[id_size];
            read_keypad_group_id(input_group_id);
            
            if (atoi(input_group_id) - atoi(group_id) == 0) {
                is_closed = 1;
            } else {
                uLCD.printf("Group ID does not match!\nUnable to close session.\n");
            }
        }
    }
    close_session();
}
/*
 * Compare whether the currently scanned fingerprint is contained
 * in the existing database. Set the this_session_id data structure
 * to 1 if yes to notify that the person has signed in for this
 * session.
 *
 * @param none
 * @return none
 */
void read_from_fingerprint_scanner() {
    int FID = -1;
    //TODO: talk to fingerprint and get an ID back 
    if (FID >= 0 & FID < group_size) {
        this_session_id[FID] = 1;
    }    
}

void close_session() {
    download_to_memory();
    send_file_via_wifi();
    // TODO: put changed data in mbed to the micro sd and other housekeeping stuff
}

/* Reads from the keypad id_size digits of input
 * until the user presses the enter button.
 * If the user presses the cancel button, discard the
 * digits read and return.
 * @param char* readable the pointer to put the input read from user
 * @return int 0 if no change made to the parameter, 1 otherwise
 */
int read_keypad_group_id(char* readable) {
    
    char from_user[id_size];

    int index = 0;
    while (1) {
        if (index < id_size) {
            from_user[index] = 0; // TODO: READ_FROM KEYPAD
        } else if (index == id_size) {
            index--;        
        }
        if (is_cancelled) {
            reset_buttons();
            return 0;
        }
        if (is_entered) {
            reset_buttons();
            from_user = readable;
            return 1;
        }
        index++;
    }
}

void upload_to_scanner() {
    //TODO:
}

void download_to_memory() {
    //TODO:
}

void send_file_via_wifi() {
    // make file

    for (int i = 0; i < group_size; i++) {
        if (this_session_id[i]) {
            // append the data to the file to be exported
        }
    }
    // send data
}

void reset_buttons() {
    is_cancelled = 0;
    is_entered = 0;
}