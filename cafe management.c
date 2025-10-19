#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function Prototypes
void admin();
void customer();

void addmenuitem();
void removemenuitem();
void displaymenu();
void viewfeedback();
void checkexpense();
void customerdetails();
void view_top_selling();
void feedback_summary();
void customer_frequency();

void viewmenu();
void placeorder();
void cancelorder(); // Renamed from canclemenu
void viewbill();
void feedback();

// struct for menu in admin
struct menuitem {
    float price;
    char name[100];
    int id;
    struct menuitem *next;
};

// struct for order process in customer
struct order{
    int id ;
    char name[100];
    float price;
    struct order *next;
};

// Global variables
char cust_name[50];
long long int cust_mob;
int cust_info_entered=0;
char cust_date[20];

struct order *orderhead = NULL;
struct menuitem *head = NULL;
int current_id = 1;

struct ExpenseDay {
    char date[20];
    float total;
    struct ExpenseDay *next;
};
struct ExpenseDay *exphead = NULL;

// File Pointers
FILE *fptr;
FILE *feed;
FILE *custdet;
FILE *expense_file;

int main()
{
    printf("\t\t\t\t|| \t WELCOME TO OUR CAFE \t ||\n");
    printf("\n");
    while(1)
    {
        int module;
        printf("Enter 1 for Admin\n");
        printf("Enter 2 for Customer\n");
        printf("Enter 3 for Exit\n");
        printf("Choice: "); // Added prompt for clarity
        if (scanf("%d",&module) != 1) { // Basic input validation
            printf("Invalid input. Please enter a number.\n");
            // Clear input buffer
            while (getchar() != '\n');
            continue;
        }

        if(module==1) admin();
        else if(module==2) customer();
        else if (module==3) exit(0);
        else printf("Invalid choice\n");
    }
    return 0;
}

// Admin module
void admin(){
    int pass;
    printf("Enter the password (int only) : ");
    if (scanf("%d",&pass) != 1) { // Basic input validation
        printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n');
        return;
    }
    printf("-------------------\n");

    if (pass==12345){
        while(1) {
            int choice;
            printf("\nEnter your choice:\n");
            printf("1 - Add menu item\n");
            printf("2 - Remove menu item\n");
            printf("3 - Display menu items\n");
            printf("4 - Display feedback\n");
            printf("5 - Check expense\n");
            printf("6 - View Customer Details\n");
            printf("7 - View Top Selling Items\n");
            printf("8 - Feedback Summary\n");
            printf("9 - Frequent Customers\n");
            printf("10 - Exit\n");
            printf("Choice: ");
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input, try again.\n");
                while (getchar() != '\n');
                continue;
            }

            switch(choice) {
                case 1: addmenuitem(); break;
                case 2: removemenuitem(); break;
                case 3: displaymenu(); break;
                case 4: viewfeedback(); break;
                case 5: checkexpense(); break;
                case 6: customerdetails(); break;
                case 7: view_top_selling(); break;
                case 8: feedback_summary(); break;
                case 9: customer_frequency(); break;
                case 10:
                    printf("Exiting Admin Menu\n");
                    printf("---------------------\n\n");
                    return ;
                default: printf("Invalid choice, try again.\n");
            }
        }
    } else {
        printf("Wrong password, please try again!\n");
        return;
    }
}

// Add menu Function
void addmenuitem() {
    struct menuitem *new_item = (struct menuitem *)malloc(sizeof(struct menuitem));
    if (new_item == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("Enter the name of the item: ");
    scanf(" %99[^\n]", new_item->name); // Added size limit for safety

    printf("Enter the price of the item: ");
    if (scanf("%f", &new_item->price) != 1) {
        printf("Invalid price input. Item not added.\n");
        free(new_item);
        while (getchar() != '\n');
        return;
    }

    new_item->id = current_id++;
    new_item->next = NULL;

    if (head == NULL) head = new_item;
    else {
        struct menuitem *temp = head;
        while (temp->next != NULL) temp = temp->next;
        temp->next = new_item;
    }

    printf("Item added successfully with ID %d.\n", new_item->id);

    fptr=fopen("Menu.txt","a");
    if (fptr==NULL){
        printf("Error: Could not open Menu.txt for writing.\n");
        return ;
    }
    fprintf(fptr,"%d %s %.2f \n",new_item->id,new_item->name,new_item->price);
    fclose(fptr);
}

// Remove menu function
void removemenuitem() {
    int removeid;
    printf("Enter id to remove item : ");
    if (scanf("%d",&removeid) != 1) {
        printf("Invalid ID input.\n");
        while (getchar() != '\n');
        return;
    }

    FILE *tempfile=fopen("Temp.txt","w");
    fptr=fopen("Menu.txt","r");

    if (fptr==NULL || tempfile==NULL){
        printf("Error opening file(s)\n");
        if (tempfile) fclose(tempfile); // Close if opened
        if (fptr) fclose(fptr);         // Close if opened
        return ;
    }

    int id;
    char name[100];
    float price;
    int found=0;

    while (fscanf(fptr,"%d %s %f",&id ,name , &price)==3){
        if (id == removeid) found=1;
        else fprintf(tempfile,"%d %s %.2f\n",id , name , price);
    }

    fclose (fptr);
    fclose (tempfile);

    remove("Menu.txt");
    if (rename("Temp.txt","Menu.txt") != 0) {
        printf("Error renaming temporary file.\n");
    }

    if (found) {
        printf("Item removed successfully!\n");
        // Update linked list as well for consistency, though files are primary storage here
        struct menuitem *current = head;
        struct menuitem *prev = NULL;
        while (current != NULL && current->id != removeid) {
            prev = current;
            current = current->next;
        }
        if (current) {
            if (prev == NULL) head = current->next;
            else prev->next = current->next;
            free(current);
        }
    }
    else printf("Item with ID %d not found.\n", removeid);
}

// Display menu function
void displaymenu() {
    fptr = fopen("Menu.txt", "r");
    if (fptr == NULL) {
        printf("Menu is empty.\n");
        return;
    }

    int id;
    char name[100];
    float price;

    printf("\n\t\t\t\tMENU CARD\n");
    printf("_________________________________________\n");
    printf("ID\tItem Name\t\tPrice\n");
    printf("_________________________________________\n");

    while (fscanf(fptr, "%d %s %f", &id, name, &price) == 3) {
        printf("%d\t%-15s\t%.2f Rs\n", id, name, price);
    }

    printf("_________________________________________\n");
    fclose(fptr);
}

// View feedback function
void viewfeedback() {
    char line[200];
    int i; // Declared at the start of the block

    feed = fopen("Feedback.txt", "r");
    if (feed == NULL) {
        printf("No Feedback available.\n");
        return;
    }
    printf("\t\t\t  || FEEDBACK || \n");
    printf("--------------------------------------------------------------------------\n");

    // Skip the header lines
    for (i=0; i<3; i++) {
        if (fgets(line, sizeof(line), feed) == NULL) break;
    }


    while (fgets(line, sizeof(line), feed)) {
        if (strstr(line, "|") == NULL) continue;
        printf("%s", line);
    }

    printf("--------------------------------------------------------------------------\n");
    fclose(feed);
}

// Check expense function
void checkexpense() {
    // Variables declared at the start of the block
    char date[20], itemname[100];
    int id;
    float price;
    struct ExpenseDay *temp;
    struct ExpenseDay *prev;
    struct ExpenseDay *ptr;
    struct ExpenseDay *tmp;
    int found; // Added for clarity

    expense_file = fopen("Expenses.txt", "r");
    if (expense_file == NULL) {
        printf("No expense records available.\n");
        return;
    }

    // Reset linked list before processing
    while (exphead != NULL) {
        tmp = exphead;
        exphead = exphead->next;
        free(tmp);
    }

    while (fscanf(expense_file, "%s %d %s %f", date, &id, itemname, &price) == 4) {
        temp = exphead;
        prev = NULL;
        found = 0;
        // Search for existing date
        while (temp != NULL) {
            if (strcmp(temp->date, date) == 0) {
                temp->total += price;
                found = 1;
                break;
            }
            prev = temp;
            temp = temp->next;
        }

        if (!found) {
            struct ExpenseDay *newDay = (struct ExpenseDay *)malloc(sizeof(struct ExpenseDay));
            if (newDay == NULL) {
                printf("Memory allocation failed for ExpenseDay!\n");
                break; // Stop processing
            }
            strcpy(newDay->date, date);
            newDay->total = price;
            newDay->next = NULL;

            if (exphead == NULL) exphead = newDay;
            else prev->next = newDay;
        }
    }
    fclose(expense_file);

    printf("\n--- Daily Expense Report ---\n");
    ptr = exphead;
    if (ptr == NULL) {
         printf("No expenses recorded after parsing file.\n");
    }
    while (ptr != NULL) {
        printf("Date: %s | Total Expense: %.2f Rs\n", ptr->date, ptr->total);
        ptr = ptr->next;
    }

    // Clean up memory (Important!)
    while (exphead != NULL) {
        tmp = exphead;
        exphead = exphead->next;
        free(tmp);
    }
}

// View customer details function
void customerdetails() {
    // Variables declared at the start of the block
    char line[300]; // Increased buffer size

    custdet = fopen("customerdetails.txt", "r");
    if (custdet == NULL) {
        printf("Customer details not available!\n");
        return;
    }

    printf("\n\t\t\t  || CUSTOMER DETAILS || \n");
    printf("--------------------------------------------------\n");
    while (fgets(line, sizeof(line), custdet)) {
        printf("%s", line);
    }
    printf("--------------------------------------------------\n");

    fclose(custdet);
}

// Customer functions
void customer() {
    // Variables declared at the start of the block
    int choice1;

    while(1) {
        printf("\nEnter your choice from below\n");
        printf("1 - View menu \n");
        printf("2 - Place order \n");
        printf("3 - Cancel Order\n"); // Updated name
        printf("4 - View bill\n");
        printf("5 - Feedback\n");
        printf("6 - Exit\n");
        printf("Choice: ");
        if (scanf("%d", &choice1) != 1) {
            printf("Invalid input, try again.\n");
            while (getchar() != '\n');
            continue;
        }

        switch(choice1) {
            case 1: viewmenu(); break;
            case 2: placeorder(); break;
            case 3: cancelorder(); break; // Updated name
            case 4: viewbill(); break;
            case 5: feedback(); break;
            case 6:
                printf("Exiting Customer Menu. Thanks! Visit again...\n\n");
                // Clear the order and customer info on exit
                while(orderhead != NULL) {
                    struct order *tmp = orderhead;
                    orderhead = orderhead->next;
                    free(tmp);
                }
                cust_info_entered = 0;
                return ;
            default: printf("Invalid choice, try again.\n");
        }
    }
}

// View menu function
void viewmenu() {
    // Variables declared at the start of the block
    int id;
    char name[100];
    float price;

    fptr = fopen("Menu.txt", "r");
    if (fptr == NULL) {
        printf("Menu not available, SORRY\n");
        return;
    }

    printf("\n\t\t\t\t|| MENU CARD ||\n");
    printf("\t\t=======================================\n");
    printf("\t\t  ID\t  Item Name\t  Price\n");
    printf("\t\t=======================================\n");

    while (fscanf(fptr, "%d %s %f", &id, name, &price) == 3) {
        printf("\t\t|%d\t|%-15s\t|%.2f Rs\n", id, name, price);
    }

    printf("\t\t========================================\n");
    fclose(fptr);
}

// Place order function
void placeorder() {
    // Variables declared at the start of the block
    int id;
    char name[100];
    float price;
    int orderid;
    int found=0;
    time_t t;
    struct tm *tm;
    struct order *neworder;
    struct order *temp;

    // Get customer info if not already done
    if (cust_info_entered==0){
        printf("Enter your name : ");
        scanf(" %49[^\n]",cust_name);

        printf("Enter your Mobile Number : ");
        if (scanf("%lld",&cust_mob) != 1) {
            printf("Invalid mobile number. Order not placed.\n");
            while (getchar() != '\n');
            return;
        }
        getchar();

        // Auto-set current date
        t = time(NULL);
        tm = localtime(&t);
        strftime(cust_date, sizeof(cust_date), "%Y-%m-%d", tm);
        printf("Date set to current date: %s\n", cust_date);

        cust_info_entered=1;
    }

    printf("Enter the id to order menu : \n");
    if (scanf("%d",&orderid) != 1) {
        printf("Invalid ID input.\n");
        while (getchar() != '\n');
        return;
    }

    fptr = fopen("Menu.txt","r");
    if (fptr==NULL){
        printf("NO Menu available \n");
        return;
    }

    while (fscanf(fptr, "%d %s %f", &id, name, &price) == 3) {
        if (orderid == id ){
            printf("Id : %d \t Name : %s \t Price : %.2f \n",id , name , price);
            printf("Item added succesfully !\n");

            neworder = (struct order*)malloc(sizeof(struct order));
            if (neworder == NULL) {
                printf("Memory allocation failed for order.\n");
                break;
            }
            neworder->id=id ;
            strcpy(neworder->name,name);
            neworder->price=price;
            neworder->next=NULL;

            if (orderhead==NULL) orderhead = neworder;
            else {
                temp=orderhead;
                while (temp->next !=NULL) temp=temp->next;
                temp->next=neworder;
            }

            found =1;

            // Write to Expenses.txt file for tracking
            expense_file = fopen("Expenses.txt", "a");
            if (expense_file == NULL) {
                printf("Error writing expense to file.\n");
            } else {
                fprintf(expense_file, "%s %d %s %.2f\n", cust_date, id, name, price); // Using cust_date
                fclose(expense_file);
            }

            break;
        }
    }
    if (!found) printf("Menu item not found with ID %d!\n", orderid);
    fclose(fptr);
}

// Cancel order function (renamed)
void cancelorder() {
    // Variables declared at the start of the block
    int cancel;
    struct order*temp;
    struct order*prev;

    if (orderhead==NULL){
        printf("No orders made ! \n");
        return ;
    }
    printf("Enter the item id to cancel your order : \n");
    if (scanf("%d",&cancel) != 1) {
        printf("Invalid ID input.\n");
        while (getchar() != '\n');
        return;
    }

    temp=orderhead;
    prev=NULL;

    while (temp!=NULL && temp->id != cancel){
        prev=temp;
        temp=temp->next;
    }

    if (temp==NULL){
        printf("Your order not found ! (ID: %d)\n", cancel);
        return;
    }
    if (prev==NULL) orderhead=temp->next;
    else prev->next=temp->next;

    printf("Your order for item ID %d removed successfully!\n", cancel);
    free(temp);
}

// View bill function
void viewbill(){
    // Variables declared at the start of the block
    struct order *temp;
    float total =0;
    long file_size;

    if (orderhead==NULL){
        printf("No order placed ! \n");
        return ;
    }
    temp=orderhead;

    printf("\n\t\t\t\t\tBILL\n");
    printf("========================================\n");
    printf("Customer: %s\t\tDate: %s\n", cust_name, cust_date);
    printf("Mobile No: %lld\n", cust_mob);
    printf("========================================\n");
    printf("ID\tItem Name\tPrice\n");
    printf("========================================\n");

    custdet = fopen("customerdetails.txt","a");
    if (custdet==NULL){
        printf("Error opening customer details file\n");
        return;
    }

    fseek(custdet, 0, SEEK_END);
    file_size = ftell(custdet);

    if (file_size == 0) fprintf(custdet,"============CUSTOMER DETAILS============\n\n");

    fprintf(custdet,"---------------------------------------------\n");
    fprintf(custdet,"Name : %s\n",cust_name);
    fprintf(custdet,"Mobile No : %lld\n",cust_mob);
    fprintf(custdet,"Date : %s\n\n",cust_date);
    fprintf(custdet,"-----------------Order details---------------\n");
    fprintf(custdet," ID\t Item Name\t Price\n");

    while (temp != NULL) {
        printf("%d\t%-10s\t%.2f\n", temp->id, temp->name, temp->price);
        fprintf(custdet,"|%d\t|%-10s\t|%.2f\n", temp->id, temp->name, temp->price);
        total += temp->price;
        temp = temp->next;
    }

    printf("========================================\n");
    printf("Total Bill: %.2f Rs\n", total);

    fprintf(custdet, "---------------------------------------------\n");
    fprintf(custdet, "Total Bill : %.2f Rs\n", total);
    fprintf(custdet, "=============================================\n\n\n");

    fclose(custdet);
}

// Feedback function
void feedback(){
    // Variables declared at the start of the block
    char feedback_text[100]; // Renamed local variable to avoid conflict
    long long int mobile;
    char pname[50];
    long file_size;

    printf("Enter your name: ");
    scanf(" %49[^\n]", pname);

    printf("Enter your mobile number: ");
    if (scanf("%lld", &mobile) != 1) {
        printf("Invalid mobile number input. Feedback not submitted.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    printf("Please enter your valuable feedback (max 99 chars): \n");
    scanf(" %99[^\n]", feedback_text);

    feed = fopen("Feedback.txt", "a+");
    if (feed == NULL) {
        printf("Feedback file is not available.\n");
        return;
    }
    fseek(feed, 0, SEEK_END);
    file_size = ftell(feed);

    if (file_size == 0) {
        fprintf(feed, "\n\t\t\tFEEDBACK\n");
        fprintf(feed, " --------------------------------------------------------------------------\n");
        fprintf(feed, "| %-20s | %-15s | %-30s|\n", "Name", "Mobile No", "Feedback");
        fprintf(feed, " --------------------------------------------------------------------------\n");
    }

    fprintf(feed, "| %-20s | %-15lld | %-30s|\n", pname, mobile, feedback_text);
    fprintf(feed, " --------------------------------------------------------------------------\n");

    fclose(feed);
    printf("\nThank You For Your Valuable Response!!!...\n");
}

// -------------------- NEW FEATURES --------------------

// 1. Top Selling Items
void view_top_selling() {
    // Variables declared at the start of the block
    FILE *exp;
    typedef struct {
        char name[100];
        int count;
    } ItemCount;
    ItemCount items[200];
    int total = 0;
    char date[20], name[100];
    int id;
    float price;
    int found;
    int i, j; // Loop control variables
    ItemCount temp_item; // Temporary variable for swap

    exp = fopen("Expenses.txt", "r");
    if (!exp) {
        printf("No expense records found! Cannot determine top selling items.\n");
        return;
    }

    while (fscanf(exp, "%s %d %s %f", date, &id, name, &price) == 4) {
        found = 0;
        for (i = 0; i < total; i++) {
            if (strcmp(items[i].name, name) == 0) {
                items[i].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
            // Check for array bounds
            if (total >= 200) {
                printf("Warning: Maximum item count reached for analysis.\n");
                continue;
            }
            strcpy(items[total].name, name);
            items[total].count = 1;
            total++;
        }
    }
    fclose(exp);

    if (total == 0) {
        printf("No sales yet.\n");
        return;
    }

    // Simple Bubble Sort to sort items by count (descending)
    for (i = 0; i < total - 1; i++) {
        for (j = i + 1; j < total; j++) {
            if (items[j].count > items[i].count) {
                temp_item = items[i];
                items[i] = items[j];
                items[j] = temp_item;
            }
        }
    }

    printf("\n===== TOP SELLING ITEMS (Up to 5) =====\n");
    printf("%-20s | %s\n", "Item Name", "Orders");
    printf("-----------------------------\n");
    for (i = 0; i < total && i < 5; i++) {
        printf("%-20s | %d\n", items[i].name, items[i].count);
    }
    printf("-----------------------------\n");
}

// 2. Feedback Sentiment Summary
void feedback_summary() {
    // Variables declared at the start of the block
    FILE *f;
    char line[300];
    int positive = 0, negative = 0, total = 0;
    const char *positiveWords[] = {"good", "great", "excellent", "nice", "amazing", "love"};
    const char *negativeWords[] = {"bad", "poor", "slow", "worst", "terrible", "late"};
    int posCount = sizeof(positiveWords)/sizeof(positiveWords[0]);
    int negCount = sizeof(negativeWords)/sizeof(negativeWords[0]);
    int i, j; // Loop control variables
    int analyzed_count;
    float positivity;

    f = fopen("Feedback.txt", "r");
    if (!f) {
        printf("No feedback data found!\n");
        return;
    }

    // Skip the header lines
    for (i=0; i<3; i++) {
        if (fgets(line, sizeof(line), f) == NULL) break;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "|") == NULL) continue;
        total++;
        // Simple word count for sentiment (very basic approach)
        for (i=0; i<posCount; i++) {
            if (strstr(line, positiveWords[i])) {
                positive++;
                break; // Count only once per feedback entry for simplicity
            }
        }
        for (j=0; j<negCount; j++) {
            if (strstr(line, negativeWords[j])) {
                negative++;
                break; // Count only once per feedback entry for simplicity
            }
        }
    }
    fclose(f);

    if (total == 0) {
        printf("No feedback entries found!\n");
        return;
    }

    printf("\n===== FEEDBACK SUMMARY (Per Entry) =====\n");
    printf("Total Feedback Entries: %d\n", total);
    printf("Entries with Positive Keywords: %d\n", positive);
    printf("Entries with Negative Keywords: %d\n", negative);

    // Calculate positivity based on positive vs negative entries found
    analyzed_count = positive + negative;
    positivity = (analyzed_count > 0) ? (100.0 * positive / analyzed_count) : 0;
    printf("Overall Positivity (Analyzed Entries): %.1f%%\n", positivity);
    printf("========================================\n");
}

// 3. Frequent Customers
void customer_frequency() {
    // Variables declared at the start of the block
    FILE *f;
    typedef struct {
        long long mobile;
        int count;
    } Cust;
    Cust customers[300];
    int total = 0;
    char line[300];
    int i, j; // Loop control variables
    long long num;
    int found;
    Cust temp_cust; // Temporary variable for swap

    f = fopen("customerdetails.txt", "r");
    if (!f) {
        printf("No customer records found!\n");
        return;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "Mobile No")) {
            // Correct format string to match the file output: "Mobile No : %lld\n"
            if (sscanf(line, "Mobile No : %lld", &num) == 1) {
                found = 0;
                for (i = 0; i < total; i++) {
                    if (customers[i].mobile == num) {
                        customers[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (total >= 300) {
                         printf("Warning: Maximum customer count reached for analysis.\n");
                         continue;
                    }
                    customers[total].mobile = num;
                    customers[total].count = 1;
                    total++;
                }
            }
        }
    }
    fclose(f);

    if (total == 0) {
        printf("No customers found!\n");
        return;
    }

    // Simple Bubble Sort to sort customers by visit count (descending)
    for (i = 0; i < total - 1; i++) {
        for (j = i + 1; j < total; j++) {
            if (customers[j].count > customers[i].count) {
                temp_cust = customers[i];
                customers[i] = customers[j];
                customers[j] = temp_cust;
            }
        }
    }

    printf("\n===== FREQUENT CUSTOMERS (Up to 5) =====\n");
    printf("%-15s | %s\n", "Mobile No", "Visits");
    printf("------------------------------\n");
    for (i = 0; i < total && i < 5; i++) {
        printf("%-15lld | %d\n", customers[i].mobile, customers[i].count);
    }
    printf("------------------------------\n");
}
