#include "neuron.hpp"

Neuron::Neuron(SDL_FPoint coords, float activation, float bias, int layer, int radius) : coords(coords), layer(layer), radius(radius), bias(bias), activation(activation) {}

void Neuron::render(SDL_Renderer* renderer, TTF_Font* font)
{
    // Set the transparency of the neuron based on the activation from 0 to 1
    float alpha = static_cast<Uint8>(activation * 255);
    SDL_SetRenderDrawColor(renderer, alpha, alpha, alpha, 255);

    // Midpoint circle algorithm
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;

    int circumferenceThickness = 2; // Set the thickness here

    while (x >= y)
    {
        SDL_RenderDrawLineF(renderer, coords.x - x, coords.y + y, coords.x + x, coords.y + y);
        SDL_RenderDrawLineF(renderer, coords.x - x, coords.y - y, coords.x + x, coords.y - y);
        SDL_RenderDrawLineF(renderer, coords.x - y, coords.y + x, coords.x + y, coords.y + x);
        SDL_RenderDrawLineF(renderer, coords.x - y, coords.y - x, coords.x + y, coords.y - x);

        y++;

        if (radiusError < 0)
        {
            radiusError += 2 * y + 1;
        }
        else
        {
            x--;
            radiusError += 2 * (y - x) + 1;
        }
    }

    for (int i = 0; i < circumferenceThickness; i++) {
        x = radius - i;
        y = 0;
        radiusError = 1 - x;

        while (x >= y)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FPoint points[8] = {
                { coords.x + x, coords.y + y },
                { coords.x + y, coords.y + x },
                { coords.x - y, coords.y + x },
                { coords.x - x, coords.y + y },
                { coords.x - x, coords.y - y },
                { coords.x - y, coords.y - x },
                { coords.x + y, coords.y - x },
                { coords.x + x, coords.y - y }
            };

            SDL_RenderDrawPointsF(renderer, points, 8);

            y++;

            if (radiusError < 0)
            {
                radiusError += 2 * y + 1;
            }
            else
            {
                x--;
                radiusError += 2 * (y - x) + 1;
            }
        }
    }

    // Draw the activation as text inside the neuron in white
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << activation;
    std::string activationString = stream.str();

    SDL_Color textColor = (activation <= 0.5) ? SDL_Color({255, 255, 255, 255}) : SDL_Color({0, 0, 0, 255});
    SDL_Surface* surface = TTF_RenderText_Blended(font, activationString.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {static_cast<int>(coords.x - radius / 1.5), static_cast<int>(coords.y - radius / 2), radius * 1.5, radius};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}