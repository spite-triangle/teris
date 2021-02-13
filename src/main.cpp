#include "SFML/Graphics.hpp"
#include "Teris.hpp"
using namespace sf;
using namespace std;

int main()
{
    configures config(0.5);

    Teris gameTeris(config);

    gameTeris.run();
    return 0;

}