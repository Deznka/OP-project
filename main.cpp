#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>

// Szkielet metaheurystyki
// oddalić sie od algorytmu zachalnnego

// jak iteracja powtarza wynik to mozna stopowac;
// przerwac gdy nie ma poprawy od X iteracji (nawet przy psuciu wyniku)
// lub przerwac po X czasu - jesli iteracja trwa za długo

// zoptymalizować żeby krócej szukało
// można wygenerować obraz grafu

// na zaliczeniu:
// na każdą instancje 3 minuty max
// wyniki mają być powtarzalne, nie że raz sie pojawi i więcej nie - wtedy tego wyniku nie uznajemy
//

using namespace std;

// ---------------------------------------------------
// GENERATOR DANYCH
// ---------------------------------------------------
void generujMiasta(int liczbaMiast, const string& nazwaPliku) {
    ofstream plikWyjsciowy(nazwaPliku);

    if (!plikWyjsciowy.is_open()) {
        cout << "Blad tworzenia pliku!" << endl;
        return;
    }

    srand(time(0));

    plikWyjsciowy << liczbaMiast << endl;

    for (int i = 1; i <= liczbaMiast; i++) {
        int x = rand() % 2000 + 1;
        int y = rand() % 2500 + 1;
        plikWyjsciowy << i << " " << x << " " << y << endl;
    }

    plikWyjsciowy.close();
    cout << "Wygenerowano " << liczbaMiast << " miast i zapisano do pliku " << nazwaPliku << endl;
}

// ---------------------------------------------------
// FUNKCJA LICZĄCA KOSZT TRASY
// ---------------------------------------------------
double policzKosztTrasy(const vector<int>& trasa, const vector<vector<double>>& D) {
    double koszt = 0.0;
    for (size_t i = 0; i < trasa.size() - 1; i++) {
        koszt += D[trasa[i]][trasa[i + 1]];
    }
    koszt += D[trasa.back()][trasa.front()]; // powrót do startu
    return koszt;
}

// ---------------------------------------------------
// ALGORYTM MRÓWKOWY (ACO)
// ---------------------------------------------------
void ACO(const vector<vector<double>>& D, int n) {

    cout << "\n=== ALGORYTM MRÓWKOWY (ACO) ===\n";

    srand(time(0));

    // PARAMETRY
    int liczbaMrowek = n;
    int iteracje = 50;
    double alfa = 1.0; // wpływ feromonów
    double beta = 5.0; // wpływ atrakcyjności krawędzi
    double rho = 0.3;  // współczynnik parowania feromonów
    double Q = 100.0;

    // FEROMONY (początkowo 1.0)
    vector<vector<double>> feromony(n + 1, vector<double>(n + 1, 1.0));

    // ATRAKCYJNOŚĆ = odwrotność odległości
    vector<vector<double>> atrakcyjnosc(n + 1, vector<double>(n + 1, 0.0));
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= n; j++)
            if (i != j) atrakcyjnosc[i][j] = 1.0 / (D[i][j] + 0.0001);

    vector<int> najlepszaTrasa;
    double najlepszyKoszt = numeric_limits<double>::infinity();

    // -------------------------------------
    // GŁÓWNA PĘTLA ACO
    // -------------------------------------
    for (int iter = 0; iter < iteracje; iter++) {

        vector<vector<int>> trasy(liczbaMrowek);
        vector<double> koszty(liczbaMrowek);

        // -------------------------------------
        // KAŻDA MRÓWKA BUDUJE TRASĘ
        // -------------------------------------
        for (int m = 0; m < liczbaMrowek; m++) {

            int start = (m % n) + 1;
            vector<int> trasa;
            vector<bool> odwiedzony(n + 1, false);

            trasa.push_back(start);
            odwiedzony[start] = true;
            int aktualne = start;

            // budowanie ścieżki
            for (int k = 1; k < n; k++) {

                vector<double> P(n + 1, 0.0);
                double suma = 0.0;

                // liczymy prawdopodobieństwa
                for (int j = 1; j <= n; j++) {
                    if (!odwiedzony[j]) {
                        double tau = pow(feromony[aktualne][j], alfa);
                        double eta = pow(atrakcyjnosc[aktualne][j], beta);
                        P[j] = tau * eta;
                        suma += P[j];
                    }
                }

                // losowanie następnego miasta
                double los = ((double)rand() / RAND_MAX) * suma;
                double akum = 0.0;
                int wybrane = -1;

                for (int j = 1; j <= n; j++) {
                    if (!odwiedzony[j]) {
                        akum += P[j];
                        if (akum >= los) {
                            wybrane = j;
                            break;
                        }
                    }
                }

                if (wybrane == -1)
                    for (int j = 1; j <= n; j++)
                        if (!odwiedzony[j]) { wybrane = j; break; }

                trasa.push_back(wybrane);
                odwiedzony[wybrane] = true;
                aktualne = wybrane;
            }

            double koszt = policzKosztTrasy(trasa, D);
            trasy[m] = trasa;
            koszty[m] = koszt;

            if (koszt < najlepszyKoszt) {
                najlepszyKoszt = koszt;
                najlepszaTrasa = trasa;
            }
        }

        // -------------------------------------
        // PAROWANIE FEROMONÓW
        // -------------------------------------
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= n; j++)
                feromony[i][j] *= (1.0 - rho);

        // -------------------------------------
        // AKTUALIZACJA FEROMONÓW (WZMACNIANIE)
        // -------------------------------------
        for (int m = 0; m < liczbaMrowek; m++) {
            double delta = Q / koszty[m];
            const auto& trasa = trasy[m];

            for (int i = 0; i < n - 1; i++) {
                int a = trasa[i];
                int b = trasa[i + 1];
                feromony[a][b] += delta;
                feromony[b][a] += delta;
            }
            feromony[trasa.back()][trasa.front()] += delta;
            feromony[trasa.front()][trasa.back()] += delta;
        }

        cout << "Iteracja " << iter + 1 << " | Najlepszy koszt: " << najlepszyKoszt << endl;
    }

    // -------------------------------------
    // WYNIK
    // -------------------------------------
    cout << "\n=== WYNIK ACO ===\nNajlepsza trasa: ";
    for (int x : najlepszaTrasa) cout << x << " ";
    cout << najlepszaTrasa.front();
    cout << "\nKoszt: " << najlepszyKoszt << "\n\n";
}

// ---------------------------------------------------
// GŁÓWNY MAIN (z Twoim kodem + mrówkowy ACO)
// ---------------------------------------------------
int main() {
    cout << "=== PROBLEM KOMIWOJAZERA ===" << endl << endl;
    cout << "Wybierz opcje:" << endl;
    cout << "1. Wygeneruj nowe dane" << endl;
    cout << "2. Uzyj istniejacych danych z pliku test.txt" << endl;
    cout << "Wybor: ";

    int wybor;
    cin >> wybor;

    string nazwaPliku;

    if (wybor == 1) {
        int liczbaMiastDoWygenerowania;
        cout << "Podaj liczbe miast: ";
        cin >> liczbaMiastDoWygenerowania;

        nazwaPliku = "generator.txt";
        generujMiasta(liczbaMiastDoWygenerowania, nazwaPliku);
        cout << endl;
    }
    else if (wybor == 2) {
        nazwaPliku = "bier127.txt";
    }
    else {
        cout << "Nieprawidlowy wybor!" << endl;
        return 1;
    }

    // -------------------------------------
    // WCZYTYWANIE DANYCH
    // -------------------------------------
    ifstream plik(nazwaPliku);
    if (!plik.is_open()) {
        cout << "Blad otwarcia pliku!" << endl;
        return 1;
    }

    int n;
    plik >> n;

    string linia;
    getline(plik, linia);

    vector<pair<double, double>> punkty(n + 1);

    while (getline(plik, linia)) {
        stringstream s(linia);
        int id; double x, y;
        if (s >> id >> x >> y)
            punkty[id] = {x, y};
    }

    plik.close();

    // -------------------------------------
    // MACIERZ ODLEGŁOŚCI
    // -------------------------------------
    vector<vector<double>> D(n + 1, vector<double>(n + 1, 0));
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= n; j++)
            if (i != j)
                D[i][j] = sqrt(pow(punkty[i].first - punkty[j].first, 2) +
                               pow(punkty[i].second - punkty[j].second, 2));

    // -------------------------------------
    // URUCHOM MRÓWKOWY ACO
    // -------------------------------------
    ACO(D, n);

    return 0;
}
