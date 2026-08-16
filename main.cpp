#include "MudraEngine.h"

int main() {
    MudraEngine mudra;

    cout << "========================================================\n";
    cout << "       MUDRA: CROSS-BORDER PAYMENT ROUTING ENGINE       \n";
    cout << "========================================================\n";

    // Register 50 global currencies across all major trade corridors
    mudra.register_currency("INR", "India");
    mudra.register_currency("USD", "USA");
    mudra.register_currency("EUR", "Eurozone");
    mudra.register_currency("GBP", "UK");
    mudra.register_currency("JPY", "Japan");
    mudra.register_currency("AED", "UAE");
    mudra.register_currency("SAR", "Saudi Arabia");
    mudra.register_currency("BRL", "Brazil");
    mudra.register_currency("LKR", "Sri Lanka");
    mudra.register_currency("GHS", "Ghana");
    mudra.register_currency("PLN", "Poland");
    mudra.register_currency("IDR", "Indonesia");
    mudra.register_currency("THB", "Thailand");
    mudra.register_currency("ZAR", "South Africa");
    mudra.register_currency("AUD", "Australia");
    mudra.register_currency("NGN", "Nigeria");
    mudra.register_currency("VND", "Vietnam");
    mudra.register_currency("CAD", "Canada");
    mudra.register_currency("CHF", "Switzerland");
    mudra.register_currency("CNY", "China");
    mudra.register_currency("KRW", "South Korea");
    mudra.register_currency("MYR", "Malaysia");
    mudra.register_currency("PHP", "Philippines");
    mudra.register_currency("RUB", "Russia");
    mudra.register_currency("TRY", "Turkey");
    mudra.register_currency("ILS", "Israel");
    mudra.register_currency("KWD", "Kuwait");
    mudra.register_currency("MXN", "Mexico");
    mudra.register_currency("COP", "Colombia");
    mudra.register_currency("ARS", "Argentina");
    mudra.register_currency("PEN", "Peru");
    mudra.register_currency("CLP", "Chile");
    mudra.register_currency("ETB", "Ethiopia");
    mudra.register_currency("KES", "Kenya");
    mudra.register_currency("NPR", "Nepal");
    mudra.register_currency("MMK", "Myanmar");
    mudra.register_currency("KHR", "Cambodia");
    mudra.register_currency("MNT", "Mongolia");
    mudra.register_currency("KZT", "Kazakhstan");
    mudra.register_currency("UZS", "Uzbekistan");
    mudra.register_currency("GEL", "Georgia");
    mudra.register_currency("AMD", "Armenia");
    mudra.register_currency("AZN", "Azerbaijan");
    mudra.register_currency("HUF", "Hungary");
    mudra.register_currency("CZK", "Czech Republic");
    mudra.register_currency("DKK", "Denmark");
    mudra.register_currency("SEK", "Sweden");
    mudra.register_currency("NOK", "Norway");
    mudra.register_currency("NZD", "New Zealand");
    mudra.register_currency("IRR", "Iran");

    // Load CSV market liquidity matrix
    if (!mudra.load_rates_from_csv("rates.csv")) {
        cout << "Terminating engine due to missing corridor data.\n";
        return 1;
    }

    // Start Interactive Terminal Interface
    mudra.start_interactive_mode();

    return 0;
}



/*    g++ -std=c++17 main.cpp MudraEngine.cpp -o mudra_app

      ./mudra_app    

 */     