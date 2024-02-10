#include "FirebaseAuth.h"

FirebaseAuthHandler Auth;

void FirebaseAuthHandler::connect(const char *email, const char *password, const char *apiKey)
{
    // Source: https://randomnerdtutorials.com/esp32-esp8266-firebase-authentication/

    // Assign the api key (required)
    config.api_key = apiKey;
    // Assign the user sign in credentials
    auth.user.email = email;
    auth.user.password = password;
    // Initialize the library with the Firebase authen and config
    Firebase.begin(&config, &auth);

    // Getting the user UID might take a few seconds
    while ((auth.token.uid) == "")
    {
        Serial.print('.');
        delay(250);
    }
}

const char *FirebaseAuthHandler::getAuthToken()
{
    if (Firebase.isTokenExpired())
        Firebase.refreshToken(&config);
    return Firebase.getToken();
}

const char *FirebaseAuthHandler::getUserId()
{
    return auth.token.uid.c_str();
}
