#include <random>
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

class Plateau{
    private:
        // Nom des cases du plateau
        vector<string> caseList ={ 
            "depart", "boulevardDeBelleville", "caisseCommunaute", "rueLecourbe", "impots", "gareMontparnasse", "rueDeVaugirard", "chance", "rueDeCourcelles", "avenueDeLaRepublique",
            "prison", "boulevardDeLaVillette", "electricite", "avenueDeNeuilly", "rueDeParadis", "gareDeLyon", "avenueMozart", "caisseCommunaute", "boulevardST-Michel", "placePigalle",
            "parc", "avenueMatignon", "chance", "boullevradMalesherbes", "avenueHenri-Martin", "gareDuNord", "foubourgST-Honore", "placeDeLaBourse", "eaux", "rueLaFayette",
            "allezPrison", "avenueDeBreteuil", "avenueFoch", "caisseCommunaute", "boulevardDesCapucines", "gareST-Lazare", "chance", "avenueDesChamps-Elysee", "taxe", "rueDeLaPaix"
        };

        // Prix de chaque case du plateau (en euro)
        vector<int> casePrice = {
            0, 60, 0, 60, -200, 200, 100, 0, 100, 120,
            0, 140, 150, 140, 160, 200, 180, 0, 180, 200, 
            0, 220, 0, 220, 240, 200, 260, 260, 150, 280,
            0, 300, 300, 0, 320, 200, 0, 350, -200, 400
        };

    public:
        string getCaseName(int position) const { return caseList[position]; }

        int getCasePrice(int position) const { return casePrice[position]; }

        pair<int, int> rollDice() const {
            random_device rd;
            uniform_int_distribution<int> dist(1, 6);
            return {dist(rd), dist(rd)};
        }

};

// Classe représentant un joueur
class Player {
  private:

    Plateau* plateau;
    string name;                           // Nom du joueur
    int money;                             // Argent disponible
    int position;                          // Position actuelle sur le plateau
    int consecutiveDouble = 0;             // Nombre de doubles consécutifs
    int dice1, dice2, moveSteps;           // Valeurs des dés et le nombre de déplacements
    vector<string> ownedProperties;        // Liste des propriétés possédées par le joueur

  public:
    /** Constructeur de la classe Player.
     * Initialise le nom, l'argent et la position du joueur.
     * @param namePlayer Le nom du joueur. 
    */
    Player(string namePlayer, Plateau* gamePlateau) : plateau(gamePlateau) {
      name = namePlayer;
      money = 1500;
      position = 0; // Position de départ
    }

    /** Vérifie si le joueur a fait un double en lançant les dés.
     * @return 'true' si un double est lancé, `false` sinon.
     */
    bool isDouble() {
      if (dice1 != dice2) {
        consecutiveDouble=0;
        return false;
      } else {
        cout << "Vous avez fait un double, vous pouvez rejouer." << endl; 
        consecutiveDouble++;
        return true;
      }
    }

    /** Affiche les propriétés possédées par le joueur et son argent.
     */
    void displayOwnedProperties() {
      cout << "Vous avez " << money << " skibidibucks.";
      if (ownedProperties.empty()) {
        cout << " Vous ne possédez pas de propriétés." << endl;
      } else {
        cout << " Vous possédez les propriétés suivantes : ";
        for (size_t i = 0; i < ownedProperties.size(); i++) {
          cout << ownedProperties[i];
          if (i != ownedProperties.size() - 1) {
            cout << ", "; // Ajouter une virgule entre les propriétés sauf pour la dernière
          }
        }
        cout << "." << endl;
      }
    }

    /** Vérifie si le joueur possède déjà la propriété de la case actuelle.
     * @return `true` si la propriété est possédée, `false` sinon.
     */
    bool ownsProperty() const {
      return find(ownedProperties.begin(), ownedProperties.end(), plateau->getCaseName(position)) != ownedProperties.end();
    }

    /** Tente d'acheter la propriété de la case actuelle si elle n'est pas possédée et si c'est possible.
     */
    void attemptPurchase() {
      // Stocker la valeur de casePrice[position] dans une variable locale
      int currentPrice = plateau->getCasePrice(position);
      string currentCase = plateau->getCaseName(position);

      if (currentPrice == 0) {
        cout << "Cette case n'est pas à vendre." << endl;
      } else if (ownsProperty()) {
        cout << "Cette propriété vous appartient déjà." << endl;
      } else if (currentPrice < 0) {
        money += currentPrice;
        cout << "Vous devez " << currentPrice << " a la banque. " << name << " a maintenant " << money << " skibidibucks." << endl;
      } else if (currentPrice > money) {
        cout << "Vous n'avez pas assez d'argent pour acheter cette case";
      } else {
        string buyInput;
        cout << "Cette case coûte " << currentPrice << ". L'acheter : (O/n)." << endl << ">>> ";
        cin >> buyInput;
        if (buyInput == "O" || buyInput == "o") {
          ownedProperties.push_back(currentCase);
          money -= currentPrice;
          cout << name << " a maintenant " << money << " skibidibucks." << endl;
        }
      }
    }

    /** Lance les dés, déplace le joueur et gère l'achat de propriétés.
     */
    void play() {
        auto [dice1, dice2] = plateau->rollDice();
        cout << "Dé 1 : " << dice1 << "   |   " << "Dé 2 : " << dice2 << "   |   " << "nombre de déplacements : " << dice1 + dice2 << endl;
        position = (position + dice1 + dice2) % 40;
        cout << name << " est sur la case " << plateau->getCaseName(position) << " (n°" << position << ")." << endl;
        displayOwnedProperties();
        attemptPurchase();
    }

    /** Retourne le nom du joueur.
     * @return 'name'.
     */
    string getName() {
      return name;
    }
};

class Game {

    private:
        Plateau plateau;
        // Liste des joueurs
        vector<Player> players;

    public:
        void startGame() {

            // 1. Définir le nombre de tours et de joueurs
            cout << endl << "------------------------------- Bienvenue dans le jeu de Monopoly simplifié ! -------------------------------" << endl << endl;

            // Demander à l'utilisateur combien de tours le jeu doit durer
            int iteration = getValidatedPositiveIntegerInput(
            "Combien d'itérations (nombre de tour(s)) voulez-vous ?"
            );

            // Demander à l'utilisateur combien de joueurs vont participer
            int numPlayers = getValidatedPositiveIntegerInput(
            "Combien y a-t-il de joueurs ?"
            );

            for (int i = 0; i < numPlayers; ++i) {
                string name;
                cout << "Nom du joueur : ";
                cin >> name;
                players.push_back(Player(name, &plateau));
            }
            for (int i = 0; i < iteration; i++) {
                cout << endl << endl << "-------------------------------" << " Tour numéro " << i + 1 << " -------------------------------" << endl;
                for (auto& player : players) {
                    cout << endl << "---------- " << player.getName() << ", c'est votre tour ----------" << endl << endl;
                    player.play();
                }
            }
        }

        /** Demande à l'utilisateur d'entrer un entier strictement positif et valide.
         * @param prompt Le message à afficher.
         * @return Un entier strictement positif.
         */
        int getValidatedPositiveIntegerInput(string prompt) {
            int value;
            do {
                cout << prompt << endl << ">>> ";
                cin >> value;
                if (cin.fail() || value <= 0) { // Si l'entrée n'est pas un entier
                cin.clear(); // Réinitialise l'état du flux
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Vide le buffer
                cout << "Erreur : veuillez entrer un entier strictement positif valide !" << endl;
                }
            } while (cin.fail() || value <= 0);
            return value;
        }
};

int main() {
    Game game;
    game.startGame();
    return 0;
}
