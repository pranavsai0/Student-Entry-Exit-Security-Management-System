#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>


#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define ORANGE "\033[38;5;214m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

// Struct to store individual timestamps
struct timest {
    time_t exit;
    time_t entry;
    struct timest *next;
};

// Struct to store student details
struct student {
    char name[100];
    char roll[50];
    struct timest *time_head; // Linked list of time entries
};

// Struct for the linked list of students
struct node {
    struct student s;
    struct node *link;
};

// Function declarations
void clear_console();
void show_logs();
void add_log(const char *message);
void append(struct node **head, struct student newst);
void add_exit_time(struct student *s);
void add_entry_time(struct student *s);
void hist(struct student *s);
void writeToFile(struct node *head, const char *filename);
void loadFromFile(struct node **head, const char *filename);
void post_login(struct student *s);
void reg(struct node **head);
void delete_user(struct node **head, const char *roll_no);
int is_roll_exists(struct node *head, const char *roll);
struct student *find_student_by_roll(struct node *head, const char *roll);
void search_by_date(struct node *head, int day, int month);
void fpage(struct node **head);


void display_students(struct node *head) {
    if (head == NULL) {
        printf(RED "No students registered yet.\n" RESET);
        return;
    }

    printf(BLUE "==============================================================\n" RESET);
    printf(YELLOW "            List of Registered Students with Status\n" RESET);
    printf(BLUE "==============================================================\n" RESET);
    printf(GREEN "| %-25s | %-15s | %-5s |\n", "Name", "Roll Number", "Status");
    printf(GREEN "|---------------------------|-----------------|-------|\n" RESET);

    struct node *temp = head;

    while (temp != NULL) {
        char status[4] = "IN"; // Default status is IN
        struct timest *time_ptr = temp->s.time_head;

        // Traverse to the last time entry
        while (time_ptr != NULL) {
            if (time_ptr->entry != 0) {
                strcpy(status, "IN"); // Update status to IN if entry time is set
            } else {
                strcpy(status, "OUT"); // Update status to OUT otherwise
            }
            time_ptr = time_ptr->next;
        }

        printf("| %-25s | %-15s | %-5s |\n", temp->s.name, temp->s.roll, status);
        temp = temp->link;
    }

    printf(BLUE "==============================================================\n" RESET);
}

// Clearing screen 
void clear_console() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Display activity logs
void show_logs() {
    clear_console();
    FILE *file = fopen("logs.txt", "r");
    if (file == NULL) {
        printf(RED"No logs found.\n" RESET);
        return;
    }

    char buffer[256];
    printf("\n---- Activity Logs ----\n");
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }
    fclose(file);
    printf("\n-----------------------\n");
}

// Append new student to linked list
void append(struct node **head, struct student newst) {
    struct node *temp = (struct node *)malloc(sizeof(struct node));
    temp->s = newst;
    temp->link = NULL;

    if (*head == NULL) {
        *head = temp;
    } else {
        struct node *ptr = *head;
        while (ptr->link) {
            ptr = ptr->link;
        }
        ptr->link = temp;
    }
}


void add_log(const char *message) {
    FILE *log_file = fopen("logs.txt", "a");
    if (log_file == NULL) {
        printf(RED "Error: Unable to open log file.\n" RESET);
        return;
    }
    time_t now = time(NULL);
    fprintf(log_file, "[%s] %s\n", ctime(&now), message);
    fclose(log_file);
}


void add_exit_time(struct student *s) {
    struct timest *time_ptr = s->time_head;
    while (time_ptr) {
        if (time_ptr->entry == 0) {
            printf(RED "An exit time is already recorded for %s.\n" RESET, s->name);
            return;
        }
        time_ptr = time_ptr->next;
    }

    struct timest *new_time = (struct timest *)malloc(sizeof(struct timest));
    new_time->exit = time(NULL);
    new_time->entry = 0;
    new_time->next = NULL;

    if (s->time_head == NULL) {
        s->time_head = new_time;
    } else {
        time_ptr = s->time_head;
        while (time_ptr->next) {
            time_ptr = time_ptr->next;
        }
        time_ptr->next = new_time;
    }
    clear_console();
    printf("exit time recorded for %s at %s", s->name, ctime(&new_time->exit));
    char log_message[200];
    snprintf(log_message, sizeof(log_message), "%s has exited", s->name);
    add_log(log_message);
}


void add_entry_time(struct student *s) {
    struct timest *time_ptr = s->time_head;
    while (time_ptr != NULL && time_ptr->next != NULL) {
        time_ptr = time_ptr->next;
    }

    if (time_ptr == NULL || time_ptr->entry != 0) {
        printf(RED "No active exit found for student %s.\n" RESET, s->name);
        return;
    }
    clear_console();
    time_ptr->entry = time(NULL);
    printf("entry time recorded for %s at %s", s->name, ctime(&time_ptr->entry));
    char log_message[200];
    snprintf(log_message, sizeof(log_message), "%s has entered", s->name);
    add_log(log_message);
}


void hist(struct student *s) {
    struct timest *time_ptr = s->time_head;

    printf(BLUE "+=================================================================+\n" RESET);
    printf(YELLOW "                          History for %s                        \n" RESET, s->name);
    printf(BLUE "+=================================================================+\n" RESET);

    
    if (time_ptr == NULL) {
        printf(RED "No history recorded yet.\n" RESET);
    } else {
        printf(GREEN "| %-30s | %-30s |\n", "Exit Time", "Entry Time");
        printf(GREEN "|--------------------------------|--------------------------------|\n" RESET);
        while (time_ptr != NULL) {
            char exit_time[30], entry_time[30];
            char center_exit[31], center_entry[31];
            

            strncpy(exit_time, ctime(&time_ptr->exit), 29);
            exit_time[24] = '\0'; 
            

            if (time_ptr->entry != 0) {
                strncpy(entry_time, ctime(&time_ptr->entry), 29);
                entry_time[24] = '\0';
            } else {
                snprintf(entry_time, sizeof(entry_time), "Not Recorded");
            }

            int exit_pad = (30 - strlen(exit_time)) / 2;
            snprintf(center_exit, sizeof(center_exit), "%*s%s%*s",
                    exit_pad, "", exit_time, 30 - strlen(exit_time) - exit_pad, "");
            
            int entry_pad = (30 - strlen(entry_time)) / 2;
            snprintf(center_entry, sizeof(center_entry), "%*s%s%*s",
                    entry_pad, "", entry_time, 30 - strlen(entry_time) - entry_pad, "");
            
            // Print formatted row
            printf("| %-30s | %-30s |\n", center_exit, center_entry);
            time_ptr = time_ptr->next;
        }
    }
    printf(BLUE "+=================================================================+\n" RESET);
}


void writeToFile(struct node *head, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf(RED "Error: Unable to open file for writing.\n" RESET);
        return;
    }

    struct node *temp = head;
    while (temp != NULL) {
        fprintf(file, "%s %s", temp->s.name, temp->s.roll);
        struct timest *time_ptr = temp->s.time_head;
        while (time_ptr != NULL) {
            fprintf(file, " %ld:%ld", time_ptr->exit, time_ptr->entry);
            time_ptr = time_ptr->next;
        }
        fprintf(file, " .\n");
        temp = temp->link;
    }

    fclose(file);
}


void loadFromFile(struct node **head, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No previous data found. Starting fresh.\n");
        return;
    }

    while (!feof(file)) {
        struct student temp_student;
        temp_student.time_head = NULL;
        struct timest *last_time = NULL;

        if (fscanf(file, "%s %s", temp_student.name, temp_student.roll) != 2) break;

        char buffer[100];
        while (fscanf(file, "%s", buffer) == 1) {
            if (strcmp(buffer, ".") == 0) break;

            long exit, entry;
            if (sscanf(buffer, "%ld:%ld", &exit, &entry) == 2 && exit > 0) {
                struct timest *new_time = (struct timest *)malloc(sizeof(struct timest));
                new_time->exit = exit;
                new_time->entry = entry;
                new_time->next = NULL;

                if (temp_student.time_head == NULL) {
                    temp_student.time_head = new_time;
                } else {
                    last_time->next = new_time;
                }
                last_time = new_time;
            }
        }

        append(head, temp_student);
    }

    fclose(file);
}

struct student *find_student_by_roll(struct node *head, const char *roll) {
    while (head != NULL) {
        if (strcmp(head->s.roll, roll) == 0) {
            return &head->s;
        }
        head = head->link;
    }
    return NULL;
}


void reg(struct node **head) {
    struct student new_student;
    printf(YELLOW "Enter name: " RESET);
    scanf("%s", new_student.name);
    printf(YELLOW "Enter roll number: " RESET);
    scanf("%s", new_student.roll);

    if (is_roll_exists(*head, new_student.roll)) {
        printf(RED "Error: Roll number already exists.\n" RESET);
        return;
    }

    new_student.time_head = NULL;
    append(head, new_student);
    printf(CYAN "User registered successfully.\n" RESET);

    char log_message[200];
    snprintf(log_message, sizeof(log_message), "%s (rollno: %s) has registered", new_student.name, new_student.roll);
    add_log(log_message);
    writeToFile(*head, "login.txt");
}


int is_roll_exists(struct node *head, const char *roll) {
    while (head != NULL) {
        if (strcmp(head->s.roll, roll) == 0) {
            return 1;
        }
        head = head->link;
    }
    return 0;
}


void delete_user(struct node **head, const char *roll_no) {
    struct node *temp = *head, *prev = NULL;

    while (temp != NULL && strcmp(temp->s.roll, roll_no) != 0) {
        prev = temp;
        temp = temp->link;
    }

    if (temp == NULL) {
        printf(RED "Error: User with roll number %s not found.\n" RESET, roll_no);
        return;
    }

    if (prev == NULL) {
        *head = temp->link;
    } else {
        prev->link = temp->link;
    }

    char log_message[200];
    snprintf(log_message, sizeof(log_message), "%s (rollno: %s) has been deleted", temp->s.name, temp->s.roll);
    add_log(log_message);
    writeToFile(*head, "login.txt");

    free(temp);
    printf(CYAN "User deleted successfully.\n" RESET);
}

const char* check_status(struct student* s) {
    if (s == NULL) {
        return "Invalid student record";
    }

    struct timest* time_ptr = s->time_head;

    // If there are no timestamps
    if (time_ptr == NULL) {
        return "INSIDE";
    }

    // Traverse to the most recent timestamp
    while (time_ptr->next != NULL) {
        time_ptr = time_ptr->next;
    }

    // Determine status based on the last record
    if (time_ptr->entry != 0 && (time_ptr->exit == 0 || time_ptr->entry > time_ptr->exit)) {
        return "INSIDE";
    } else {
        return "OUTSIDE";
    }
}


void post_login(struct student *s) {
    while (1) {
        printf(BLUE "+========================================+\n" RESET);
        printf(YELLOW "|           Welcome, %-20s|\n" RESET, s->name);
        printf(BLUE "+========================================+\n" RESET);
        printf(ORANGE "|" RESET "1. " GREEN "Add Exit Time" RESET                      ORANGE"                        |\n" RESET);
        printf(ORANGE "|" RESET "2. " GREEN "Add Entry Time" RESET                     ORANGE"                       |\n" RESET);
        printf(ORANGE "|" RESET "3. " GREEN "View History" RESET                       ORANGE"                         |\n" RESET);
        printf(ORANGE "|" RESET "4. " GREEN "Check Current Status" RESET                     ORANGE"                 |\n" RESET);
        printf(ORANGE "|" RESET "5. " GREEN "Logout" RESET                             ORANGE"                               |\n" RESET);
        printf(BLUE "+========================================+\n" RESET);
        printf(YELLOW "Enter your choice: " RESET);

        int choice;
        
        // Read user input for choice
        if (scanf("%d", &choice) != 1) {
            // If scanf fails (non-integer input), clear the buffer
            while (getchar() != '\n');
            printf(RED "Invalid input. Please enter a valid choice (1-5).\n" RESET);
            continue;
        }

        switch (choice) {
            case 1:
                add_exit_time(s);
                break;
            case 2:
                add_entry_time(s);
                break;
            case 3:
                clear_console();
                hist(s);
                break;
            case 4:
                clear_console();
                printf(YELLOW "Current status : " RESET "%s\n", check_status(s));
                break;
            case 5:
                clear_console();
                printf(GREEN "Logging out. Goodbye, %s!\n" RESET, s->name);
                return;
            default:
                clear_console();
                printf(RED "Invalid choice. Try again.\n" RESET);
        }
    }
}


void search_by_date(struct node *head, int day, int month) {
    struct node *temp = head;
    int found = 0;

    printf(BLUE "\n==== Students for %02d/%02d ====\n" RESET, day, month);
    while (temp != NULL) {
        struct timest *time_ptr = temp->s.time_head;
        while (time_ptr != NULL) {
            struct tm *exit_time = localtime(&time_ptr->exit);
            if (exit_time->tm_mday == day && exit_time->tm_mon + 1 == month) {
                printf(GREEN "Name: %s, Roll: %s\n" RESET, temp->s.name, temp->s.roll);
                found = 1; 
                break;
            }
            time_ptr = time_ptr->next;
        }
        temp = temp->link;
    }

    if (!found) {
        printf(RED "No students found for the given date.\n" RESET);
    }
}

void time_diff(struct node *head)
{   
    clear_console();
    printf("Enter the minimum time difference (in seconds): " );
    int t;
    scanf("%d", &t);

    struct node *temp = head; // Start at the head of the linked list
    while (temp != NULL)
    {
        struct timest *time_ptr = temp->s.time_head; // Access the time list
        while (time_ptr != NULL)
        {
            double diff = 0;

            // Check if entry is valid and calculate diff
            if (time_ptr->entry > 0) // Ensure entry is a valid time_t value
            {
                diff = difftime(time_ptr->entry, time_ptr->exit);
            }
            else
            {
                diff = difftime(time_ptr->exit, time(NULL)); // Use current time if entry is invalid
            }


            // Check if the time difference exceeds the threshold
            if (diff >= t)
            {
                // Print the required details
                printf("Student Name: %s\n", temp->s.name);
                // printf("Roll Number: %d\n", temp->s.roll);
                printf("Entry Time: %s", time_ptr->entry > 0 ? ctime(&time_ptr->entry) : "N/A");
                printf("Time Difference: %.2f seconds\n", diff);
            }

            time_ptr = time_ptr->next; // Move to the next timestamp
        }

        temp = temp->link; // Move to the next student in the linked list
    }
}

void no_of_students(struct node *head) {
    int x=0;
    struct node *temp = head;
    while (temp != NULL) {
        temp = temp->link;
        x++;
    }
    printf(GREEN "Number of students registered is : %d\n" RESET,x);
}

// Main menu
void fpage(struct node **head) {
    int f_num=0;
    while (1) {
        if (f_num==0){
            printf(BLUE "+========================================+\n" RESET);                                                                                           
            printf(YELLOW "|        Welcome to IIITB MyGate         |\n" RESET);
            printf(BLUE "+========================================+\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 1. " GREEN "Login" RESET                                ORANGE"                               |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 2. " GREEN "Register a new user" RESET                  ORANGE"                 |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 3. " GREEN "Search by date" RESET                       ORANGE"                      |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 4. " GREEN "Delete a user" RESET                        ORANGE"                       |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 5. " GREEN "View Logs" RESET                            ORANGE"                           |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 6. " GREEN "Display Registered Students" RESET          ORANGE"         |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 7. " GREEN "Check Status" RESET                                ORANGE"                        |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 8. " GREEN "Time Diff Field Search" RESET                                ORANGE"              |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET " 9. " GREEN "Number of students" RESET                                ORANGE"                  |\n" RESET);
            sleep(1);
            printf(ORANGE"|" RESET "10. " GREEN "Exit" RESET                                ORANGE"                                |\n" RESET);
            printf(BLUE "+========================================+\n" RESET);
            printf(YELLOW "Enter your choice: " RESET                   );
            f_num++;
        }
        else {
            printf(BLUE "+========================================+\n" RESET);                                                                                           
            printf(YELLOW "|        Welcome to IIITB MyGate         |\n" RESET);
            printf(BLUE "+========================================+\n" RESET);
            printf(ORANGE"|" RESET " 1. " GREEN "Login" RESET                                ORANGE"                               |\n" RESET);
            printf(ORANGE"|" RESET " 2. " GREEN "Register a new user" RESET                  ORANGE"                 |\n" RESET);
            printf(ORANGE"|" RESET " 3. " GREEN "Search by date" RESET                       ORANGE"                      |\n" RESET);
            printf(ORANGE"|" RESET " 4. " GREEN "Delete a user" RESET                        ORANGE"                       |\n" RESET);
            printf(ORANGE"|" RESET " 5. " GREEN "View Logs" RESET                            ORANGE"                           |\n" RESET);
            printf(ORANGE"|" RESET " 6. " GREEN "Display Registered Students" RESET          ORANGE"         |\n" RESET);
            printf(ORANGE"|" RESET " 7. " GREEN "Check Status" RESET                                ORANGE"                        |\n" RESET);
            printf(ORANGE"|" RESET " 8. " GREEN "Time Diff Field Search" RESET                                ORANGE"              |\n" RESET);
            printf(ORANGE"|" RESET " 9. " GREEN "Number of students" RESET                                ORANGE"                  |\n" RESET);
            printf(ORANGE"|" RESET "10. " GREEN "Exit" RESET                                ORANGE"                                |\n" RESET);
            printf(BLUE "+========================================+\n" RESET);
            printf(YELLOW "Enter your choice: " RESET                   );       
        }
        int choice;
        

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            printf(RED "Invalid input. Please enter a valid choice (1-10).\n" RESET);
            continue;
        }


        printf(BLUE "+========================================+\n" RESET);

        switch (choice) {
            case 1: {
                clear_console();
                char roll[50];
                printf(YELLOW "Enter your roll number: " RESET);
                scanf("%s", roll);
                struct student *s = find_student_by_roll(*head, roll);
                if (s != NULL) {
                    clear_console();
                    post_login(s);
                } else {
                    printf(RED "Error: Roll number not found.\n" RESET);
                }
                break;
            }
            case 2:
                clear_console();
                reg(head);
                break;
            case 3: {
                clear_console();
                int day, month;

                // Loop to ensure valid month input
                while (1) {
                    printf(YELLOW "Enter month (1-12) : " RESET);
                    if (scanf("%d", &month) != 1) {
                        while (getchar() != '\n');
                        clear_console();
                        printf(RED "Invalid month input. Try again.\n" RESET);
                        continue;
                    }

                    if (month < 1 || month > 12) {
                        clear_console();
                        printf(RED "Invalid month input. Try again.\n" RESET);
                        continue;
                    }

                    break; // Exit the loop if the month is valid
                }
                clear_console();
                // Loop to ensure valid day input
                while (1) {
                    printf(YELLOW "Enter month (1-12) : " RESET "%d\n", month);
                    printf(YELLOW "Enter day (1-31) : " RESET);
                    if (scanf("%d", &day) != 1) {
                        while (getchar() != '\n');
                        clear_console();
                        printf(RED "Invalid day input. Try again.\n" RESET);
                        continue;
                    }
                    
                    if (day < 1 || day > 31) {
                        clear_console();
                        printf(RED "Invalid day input. Try again.\n" RESET);
                        continue;
                    }
                    
                    break; // Exit the loop if the day is valid
                }


                // Call the function to search by date once valid input is received
                search_by_date(*head, day, month);
                break;
            }
            case 4: {
                clear_console();
                char roll_no[50];
                printf( YELLOW "Enter roll number to delete: " RESET);
                scanf("%s", roll_no);
                delete_user(head, roll_no);
                break;
            }
            case 5:
                clear_console();
                show_logs();
                break;
            case 6:
                clear_console();
                display_students(*head);
                break;
            case 7:
            {
                clear_console();
                char roll[50];
                printf(YELLOW "Enter roll number to check status: " RESET);
                scanf("%s", roll);

                struct student* s = find_student_by_roll(*head, roll);
                if (s != NULL) {
                    const char* status = check_status(s); // Get current status
                    printf(YELLOW "Current Status of %s (%s): " RESET "%s\n", s->name, s->roll, status);
                    } else {
                        printf(RED "Error: Roll number not found.\n" RESET);
                    }
                break;
            }
            case 8:
            {
                   clear_console();
                   time_diff(*head);
                   break;
            }
            case 9:
                clear_console();
                no_of_students(*head);
                break;
            case 10:
            {
                clear_console();
                writeToFile(*head, "login.txt");
                printf(GREEN "Exiting the program. Goodbye!\n" RESET);
                return;
            }
            default:
                clear_console();
                printf(RED "Invalid input. Please enter a valid choice (1-10).\n" RESET);
        }
    }
}

int main() {
    struct node *head = NULL;
    clear_console();

    printf("\033[1;34m");

    printf("                 .......                                                           \n");
    printf("          ...:=++++=.=++++=:..                                                     \n");
    printf("         .-++++++++=.=+++++++=-.                                                 \n");
    printf("       .=++++++++++=.=++++++++++-.                                               \n");
    printf("     .-:.:=++++++++=.=+++++++++-.::                                              \n");
    printf("    .-++++=...-=+++=.=++++=-..:++++-.                                            \n");
    printf("    -++++++++++-::......::-+++++++++:.                                           \n");
    printf("   .+++++++++++++++=.=+++++++++++++++.                                           \n");
    printf("  .=+++++++++++++++=.=+++++++++++++++:                                           \n");
    printf("  .++++++++++++++++=.=+++++++++++++++=                                           \n");
    printf("  .::::::::::::::::...:::::::::::::::.                                           \n");
    printf("  .=+++++++++++++++=.=+++++++++++++++-                                           \n");
    printf("   :+++++++++++++++=.=+++++++++++++++.                                           \n");
    printf("    =++++++++++++++-.=++++++++++++++-.                                           \n");
    printf("    .=+++++=-:..:--:.----:..:-+++++=.                     .:::::::.               \n");
    printf("     .++-..-=++++++=.=+++++++=..:+=.   .*#######.         .#######.               \n");
    printf("       .=++++++++++=.=++++++++++=.     .*#######.         .#######.               \n");
    printf("        .:+++++++++=.=+++++++++:.      .*#######.         .#######.               \n");
    printf("          ..-++++++=.=++++++-.     .*###############.     .#######.  ......       \n");
    printf("              ..:--:.---:..        .-==+########+===.     .#######.-+######+-.    \n");
    printf("                                       .*#######.         .##################*.   \n");
    printf("    :-------:. .:-------:  .-------:   .*#######.         .########*...+#######:  \n");
    printf("    -#######=. .+#######+  -#######+   .*#######.         .#######-.    .*#####*: \n");
    printf("    -#######+. .+#######+  -#######+   .*#######.  ...... .######+.      -######=. \n");
    printf("    -#######+. .+#######+  -#######+   .*#######.  ####*. .######=       -######=. \n");
    printf("    -#######+. .+#######+  -#######+   .*#######.  ####*. .######*.     .=######-. \n");
    printf("    -#######+. .+#######+  -#######+   .*#######.  ...... .#######+.   .=######*. \n");
    printf("    -#######+. .+#######+  -#######+   .*#######.         .#########***########:  \n");
    printf("    -#######+. .+#######+  -#######+   .*#######.         .#######+##########*.   \n");
    printf("    :=======:. .-=======-  :=======-   .-=======.         .=======..=*####*=..    \n");

    
    printf("\033[0m");

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j <= 100; j++) {
            printf(CYAN "\rPlease wait while the Program starts... %d%%" RESET, j); 
            fflush(stdout);
            usleep(50000);
        }
    }
    clear_console();
    loadFromFile(&head, "login.txt");
    fpage(&head);
    return 0;
}

