#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Constants
#define DISCOUNT_MIN_AGE 18
#define DISCOUNT_MAX_AGE 28
#define DISCOUNT_PERCENTAGE 0.3f
#define MAX_SALES 10

// Menu Options
#define MENU_OPTION_BUY 'a'
#define MENU_OPTION_VIEW_STATS 'b'
#define MENU_OPTION_FEEDBACK 'c'
#define MENU_OPTION_VIEW_STOCK 'd'
#define MENU_OPTION_EXIT 'x'

#define TRUE 1
#define FALSE 0

// file
#define CAR_SALES_FILE "car_sales.csv"
FILE* file;

// Functions
void customerFeedback();
void viewCarsStock(char *carModels[], int carYears[], unsigned short totalCarsPerModel[], int numCars);
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Functions - apply discount
void applyDiscount(float *totalPrice, unsigned short userAge, unsigned short *giveDiscount) {
    *giveDiscount = FALSE;

    if (userAge >= DISCOUNT_MIN_AGE && userAge <= DISCOUNT_MAX_AGE) {
        *giveDiscount = TRUE;
        *totalPrice *= (1 - DISCOUNT_PERCENTAGE);
    } else {
        char loyaltyCardResponse;
        printf("Do you have a loyalty card? (Y/N): ");
        scanf(" %c", &loyaltyCardResponse);

        if (loyaltyCardResponse == 'Y' || loyaltyCardResponse == 'y') {
            *giveDiscount = TRUE;
            *totalPrice *= (1 - DISCOUNT_PERCENTAGE);
        }
    }
}
void viewCarsStock(char *carModels[], int carYears[], unsigned short totalCarsPerModel[], int numCars) {
    for (int i = 0; i < numCars - 1; i++) {
        for (int j = 0; j < numCars - i - 1; j++) {
            if (carYears[j] < carYears[j + 1]) {
                int tempYear = carYears[j];
                carYears[j] = carYears[j + 1];
                carYears[j + 1] = tempYear;

                char *tempModel = carModels[j];
                carModels[j] = carModels[j + 1];
                carModels[j + 1] = tempModel;

                unsigned short tempCount = totalCarsPerModel[j];
                totalCarsPerModel[j] = totalCarsPerModel[j + 1];
                totalCarsPerModel[j + 1] = tempCount;
            }
        }
    }
    printf("\n View Car Stock (Filter- Year Descending):\n");
    for (int i = 0; i < numCars; i++) {
        printf("Model: %s | Year: %d | Available: %hu\n", carModels[i], carYears[i], totalCarsPerModel[i]);
    }
}

void saveSales(
    unsigned short numberOfSales,
    unsigned short carAmountPerSale[],
    unsigned short carTypePerSale[],
    unsigned short discountGivenPerSale[],
    float salesHistory[],
    float originalPrices[],
    char customerNames[][301],
    unsigned short customerAges[],
    float discountValues[],
    char purchaseDates[][11],
    char paymentMethod[][20]
    ) {
    file = fopen(CAR_SALES_FILE, "w");
    if (file == NULL) {
        printf("Error saving the file.\n");
        return;
    }
    for (int i = 0; i < numberOfSales; i++) {
        fprintf(file, "%hu,%hu,%hu,%.2f,%.2f,%hu,%.2f,%s,%s,%s\n",
            carAmountPerSale[i],
            carTypePerSale[i],
            discountGivenPerSale[i],
            originalPrices[i],
            salesHistory[i],
            customerAges[i],
            discountValues[i],
            purchaseDates[i],
            customerNames[i],
            paymentMethod[i]
        );
    }
    fclose(file);
}
unsigned short loadSales(
    unsigned short *numberOfSales,
    unsigned short carAmountPerSale[],
    unsigned short carTypePerSale[],
    unsigned short discountGivenPerSale[],
    float salesHistory[],
    float originalPrices[],
    char customerNames[][301],
    unsigned short customerAges[],
    float discountValues[],
    char purchaseDates[][11],
    char paymentMethod[][20]
) {
    file = fopen(CAR_SALES_FILE, "r");
    if (file == NULL) return 0;

    *numberOfSales = 0;
    while (*numberOfSales < MAX_SALES &&
        fscanf(file, "%hu,%hu,%hu,%f,%hu,%f,%10[^,],%300[^,],%19[^\n]\n",
            customerNames[*numberOfSales],
            &carAmountPerSale[*numberOfSales],
            &carTypePerSale[*numberOfSales],
            &originalPrices[*numberOfSales],
            &salesHistory[*numberOfSales],
            &discountValues[*numberOfSales],
            purchaseDates[*numberOfSales],
            paymentMethod[*numberOfSales]
            ) == 10) {
        (*numberOfSales)++;
    }

    fclose(file);
    return *numberOfSales;
}

void buyCars(
    unsigned short *carsAvailable,
    unsigned short *carsSold,
    unsigned short *numberOfSales,
    unsigned short carAmountPerSale[],
    unsigned short carTypePerSale[],
    unsigned short discountGivenPerSale[],
    float salesHistory[],
    float originalPrices[],
    char customerNames[][301],
    char paymentMethod[][20],
    float carPrices[],
    char *carModels[],
    int carYears[],
    unsigned short totalCarsPerModel[],
    unsigned short customerAges[],
    float discountValues[],
    char purchaseDates[][11]
){
    if (*carsAvailable == 0) {
        printf("Sorry, we have no more cars available.\n");
        return;
    }

    unsigned short carsNeeded;
    printf("How many cars do you want [max 10]? Amount: ");
    scanf("%hu", &carsNeeded);

    if (carsNeeded > 10) {
        printf("You cannot buy more than 10 cars per person. Try Again.\n");
        return;
    }

    if (carsNeeded == 0){
        printf("Invalid amount.\n");
        return;
    }

    carAmountPerSale[*numberOfSales] = carsNeeded;

    // show available car models
    printf("\nAvailable Car Models:\n");
    for (int i = 0; i < 3; i++)
        printf("%d - %s : %.2f GBP\n", i, carModels[i], carPrices[i]);

    // Ask for the model of the car
    unsigned short carType;
    printf("Choose the model of the car (0-2): ");
    scanf("%hu", &carType);
    if (carType > 2) {
        printf("Invalid car type.\n");
        return;
    }
    if (carsNeeded > totalCarsPerModel[carType]) {
        printf("Not enough cars available of this model.\n");
        return;
    }
    carTypePerSale[*numberOfSales] = carType;

    // Show car Years before asking
    printf("\nAvailable Car Years:\n");
    for (int i = 0; i < 3; i++)
        printf("%d - %d\n", i, carYears[i]);

    unsigned short carYear;
    printf("Choose the year of the car (0-2): ");
    scanf("%hu", &carYear);
    if (carYear > 2) carYear = 2;

    // Customer name
    char customerName[301];
    printf("Enter your full name: ");
    scanf(" %[^\n]", customerName);
    strcpy(customerNames[*numberOfSales], customerName);

    // Payment Method
    int paymentChoice;
    printf("\nPayment Method:\n");
    printf("1. Cash\n2. Card\n3. Finance\nChoose (1-3): ");
    scanf("%d", &paymentChoice);

    switch (paymentChoice) {
        case 1: strcpy(paymentMethod[*numberOfSales], "Cash");
            break;
        case 2: strcpy(paymentMethod[*numberOfSales], "Card");
            break;
        case 3: strcpy(paymentMethod[*numberOfSales], "Finance");
            break;
        default: strcpy(paymentMethod[*numberOfSales], "Unknown");
    }

    // Price calculation based on year
    float basePrice = carPrices[carType];
    int baseYear = 2023;      // starting year
    float yearIncreasePercent = 0.27f;   // prices increases by 27% each year
    int yearDiff =  carYears[carYear] - baseYear;
    float totalPrice = carsNeeded * basePrice * (1 + yearIncreasePercent * yearDiff);
    // save original price before discount
    originalPrices[*numberOfSales] = totalPrice;

    unsigned short userAge;
    bool giveDiscount;
    printf("Enter your age: ");
    scanf("%hu", &userAge);
    applyDiscount(&totalPrice, userAge, &giveDiscount);
    discountGivenPerSale[*numberOfSales] = giveDiscount;

    // record customer age
    customerAges[*numberOfSales] = userAge;

    if (giveDiscount) {
        discountValues[*numberOfSales] = originalPrices[*numberOfSales] - totalPrice;
    } else {
        discountValues[*numberOfSales] = 0.0f;
    }
    printf("Enter the date of purchase: ");
    scanf(" %10s", purchaseDates[*numberOfSales]);


    // Insurance section
    char userWantsInsurance;
    float insuranceCost = 0.0f;
    printf("\nDo you want insurance? (Y/N): ");
    scanf(" %c", &userWantsInsurance);
    if (userWantsInsurance == 'Y' || userWantsInsurance == 'y') {
        int insuranceDuration;
        printf("\nInsurance Duration:\n1. 1 year\n2. 2 years\n3. 3 years\nChoose: ");
        scanf("%d", &insuranceDuration);
        switch (insuranceDuration) {
            case 1: insuranceCost += 500.0f;
                break;
            case 2: insuranceCost += 900.0f;
                break;
            case 3: insuranceCost += 1400.0f;
                break;
            default: printf("Invalid choice, no insurance added.\n");
        }

        int insuranceType;
        printf("\nInsurance Cover Type:\n1. Third Party Only\n2. Third Party Fire & Theft\n3. Comprehensive\nChoose: ");
        scanf("%d", &insuranceType);
        switch (insuranceType) {
            case 1: break;
            case 2: insuranceCost += 200.0f;
                break;
            case 3: insuranceCost += 500.0f;
                break;
            default: printf("Invalid choice, default choice to Third Party Only.\n");
        }

        char breakdownCover, courtesyCar;
        printf("\nDo you want breakdown cover? (Y/N): ");
        scanf(" %c", &breakdownCover);
        if (breakdownCover == 'Y' || breakdownCover == 'y') insuranceCost += 150.0f;

        printf("Do you want a courtesy car? (Y/N) ");
        scanf(" %c", &courtesyCar);
        if (courtesyCar == 'Y' || courtesyCar == 'y') insuranceCost += 100.0f;
        printf("\nInsurance cost per car: %.2f GBP\n", insuranceCost);

        printf("\nInsurance Summary:\n");
        printf("Duration %s\n", insuranceDuration == 1 ? "1 Year" : insuranceDuration == 2 ? "2 Years" : "3 Years");
        printf("Cover Type: %s\n", insuranceType == 1 ? "Third Party Only" : insuranceType == 2 ? "Third Party, Fire & Theft" : "Comprehensive");
        printf("Breakdown Cover: %s\n", (breakdownCover == 'Y' || breakdownCover == 'y')?"Yes":"No");
        printf("Courtesy Car: %s\n", (courtesyCar == 'Y' || courtesyCar == 'y')?"Yes":"No");
    } else {
        printf("No insurance selected.\n");
    }

    // Warranty section
    int warrantyChoice;
    float warrantyCost = 0.0f;
    printf("\nWarranty Options: \n0. Standard\n1. 1 year extended\n2. 2 year extended\n3. 3 year extended\nChoose: ");
    scanf("%d", &warrantyChoice);
    switch (warrantyChoice) {
        case 0: printf("Warranty: Standard\n");
            break;
        case 1: warrantyCost = 300.0f;
            printf("Warranty: 1 year Extended\n");
            break;
        case 2: warrantyCost = 500.0f;
            printf("Warranty: 2 years extended\n");
            break;
        case 3: warrantyCost = 700.0f;
            printf("Warranty: 3 years extended\n");
            break;
        default: printf("Invalid warranty option\n");
    }
    // Add insurance and warranty to the total price
    totalPrice += carsNeeded * (insuranceCost + warrantyCost);

    // Update totals
    totalCarsPerModel[carType] -= carsNeeded;
    *carsSold += carsNeeded;
    *carsAvailable -= carsNeeded;
    salesHistory[*numberOfSales] = totalPrice;
    (*numberOfSales)++;

    printf("\nCongrats on your purchase!\n");
    printf("\nYou have bought %hu car(s)\n", carsNeeded);
    printf("Model: %s | Year: %d\n", carModels[carType], carYears[carYear]);
    printf("Original Price is %.2f GBP\n", originalPrices[*numberOfSales - 1]);
    printf("Final Price is %.2f GBP.\n", totalPrice);
    printf("Payment Method: %s\n", paymentMethod[*numberOfSales - 1]);
    printf("There are now %hu cars remaining.\n", *carsAvailable);
}

void viewStats(
    unsigned short numberOfSales,
    unsigned short carsSold,
    unsigned short carAmountPerSale[],
    unsigned short carTypePerSale[],
    unsigned short discountGivenPerSale[],
    float salesHistory[],
    float originalPrices[],
    char customerNames[][301],
    char paymentMethod[][20],
    char *carModels[],
    unsigned short customerAges[],
    float discountValues[],
    char purchaseDates[][11]
) {
    if (numberOfSales == 0) {
        printf("There are no sales yet. Cars sold: 0, Total Value: 0.00 GBP\n");
        return;
    }

    float totalSalesValue = 0;
    for (int i = 0; i < numberOfSales; i++) {
        totalSalesValue += salesHistory[i];

        printf("\nSale Index %d:\n", i);
        printf("Customer: %s\n", customerNames[i]);
        printf("Customer Age: %hu\n", customerAges[i]);
        printf("Cars Bought: %hu\n", carAmountPerSale[i]);
        printf("Model: %s\n", carModels[carTypePerSale[i]]);
        printf("Original Price: %.2f GBP\n", originalPrices[i]);
        printf("Final Price: %.2f GBP\n", salesHistory[i]);
        printf("Payment Method: %s\n", paymentMethod[i]);
        if (discountGivenPerSale[i]) {
            printf("Discount applied: Yes (30%%)\n");
        }else {
            printf("Discount Applied: No\n");
            printf("Payment Method: %s\n", paymentMethod[i]);
            printf("Discount Value: %.2f GBP\n", discountValues[i]);
            printf("Purchase Date: %s\n", purchaseDates[i]);
        }
    }

    printf("\nTOTAL CARS SOLD: %hu\n", carsSold);
    printf("TOTAL SALES VALUE: %.2f GBP\n", totalSalesValue);
}

    // Customer feedback
    void customerFeedback() {
        char name[200];
        char feedback[500];
        int rating;

        printf("\n Customer Feedback \n");
        printf("Please enter your name: ");
        scanf(" %[^\n]", name);
        printf("Rate our service (1-5): ");
        scanf("%d", &rating);
        if (rating < 1 || rating > 5) {
            printf("Invalid Rating. It must be between 1 and 5. Please input another value.\n");
            return;
        }

        printf("Enter your feedback: ");
        scanf(" %[^\n]", feedback);

        printf("\nThank you so much for your feedback!\n");
        printf("Name: %s\n", name);
        printf("Rating: %d/5\n", rating);
        printf("Feedback: %s\n", feedback);
    }

    // start of main program
    int main() {
        // car information arrays
        float carPrices[3] = {95000.0f, 78000.0f, 66000.0f};
        char *carModels[3] = {"Audi Rs7", "BMW i8", "Mercedes CLE"};
        int carYears [3] = {2025, 2024, 2023};

        (void)carYears;

        // tracking sales arrays
        unsigned short carAmountPerSale[MAX_SALES];
        unsigned short carTypePerSale[MAX_SALES];
        unsigned short discountGivenPerSale[MAX_SALES];
        float salesHistory[MAX_SALES];
        float originalPrices[MAX_SALES];
        char customerNames[MAX_SALES][301];
        char paymentMethod[MAX_SALES][20];
        unsigned short customerAges[MAX_SALES];
        float discountValues[MAX_SALES];
        char purchaseDates[MAX_SALES][11];

        // variables
        unsigned short numberOfSales = 0;
        unsigned short carsAvailable = 1000;
        unsigned short carsAvailablePerModel[3] = {1000, 1000, 1000};
        unsigned short carsSold = 0;
        char option;

        // load previous sales
        numberOfSales = loadSales(&numberOfSales, carAmountPerSale, carTypePerSale, discountGivenPerSale,
        salesHistory, originalPrices, customerNames, customerAges, discountValues,
        purchaseDates, paymentMethod);

        /* do menu loop */
        do{
            printf("Welcome to the Car Sales Project!\n\n");
            printf("Cars Available right now: %hu\n\n", carsAvailable);
            printf("Menu: \n");
            printf("a. Buy Cars\n");
            printf("b. View Sales Stats\n");
            printf("c. Customer Feedback\n");
            printf("d. View Stock\n");
            printf("x. Exit\n\n");
            printf("Choose an option: ");
            scanf(" %c", &option);

            switch (option) {
                case MENU_OPTION_BUY:
                    buyCars(&carsAvailable, &carsSold, &numberOfSales,
                        carAmountPerSale, carTypePerSale,discountGivenPerSale,
                        salesHistory, originalPrices, customerNames, paymentMethod,
                        carPrices, carModels, carYears, carsAvailablePerModel, customerAges, discountValues, purchaseDates);
                    break;

                case MENU_OPTION_VIEW_STATS:
                    viewStats(numberOfSales, carsSold, carAmountPerSale, carTypePerSale,
                        discountGivenPerSale, salesHistory, originalPrices, customerNames,
                        paymentMethod, carModels, customerAges, discountValues, purchaseDates);
                    break;

                case MENU_OPTION_FEEDBACK:
                    customerFeedback();
                    break;

                case MENU_OPTION_VIEW_STOCK:
                    viewCarsStock(carModels, carYears, carsAvailablePerModel, 3);
                    break;

                case MENU_OPTION_EXIT:
                    printf("\nThank you for using the Car Sales Program. GoodBye!\n");
                    break;

                default:
                    printf("\nInvalid option selected.\n");
            }

            if (option != MENU_OPTION_EXIT) {
                printf("\nPress Enter to return to the Menu");
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                getchar();
            }
        } while (option != MENU_OPTION_EXIT);

        system ("cls");
        printf("\n\nHave a great day!\n\n");
        return 0;
    }
