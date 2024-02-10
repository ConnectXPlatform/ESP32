#pragma once
#include <FirebaseESP32.h>

class FirebaseAuthHandler
{

public:
    void connect(const char *email, const char *password, const char *apiKey);

    const char *getAuthToken();

    const char *getUserId();

private:
    FirebaseAuth auth;
    FirebaseConfig config;
};

extern FirebaseAuthHandler Auth;
