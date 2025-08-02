#include "CommandManagerService.h"

CommandManagerService::CommandManagerService(ICommandProcessor *cmdProc, ILogger *log)
    : commandProcessor(cmdProc), logger(log), bufferIndex(0)
{
    memset(inputBuffer, 0, sizeof(inputBuffer));
}

void CommandManagerService::initialize()
{
    if (logger)
    {
        logger->info("Command Manager Service initialized");
    }
    bufferIndex = 0;
}

void CommandManagerService::update()
{
    processSerialInput();
}

void CommandManagerService::processSerialInput()
{
    while (Serial.available() > 0)
    {
        char c = Serial.read();

        if (c == '\n' || c == '\r')
        {
            if (bufferIndex > 0)
            {
                inputBuffer[bufferIndex] = '\0';
                executeCommand(inputBuffer);
                bufferIndex = 0;
            }
        }
        else if (bufferIndex < sizeof(inputBuffer) - 1)
        {
            inputBuffer[bufferIndex++] = c;
        }
        else
        {
            // 입력이 버퍼 크기를 초과한 경우
            inputBuffer[sizeof(inputBuffer) - 1] = '\0';
            Serial.println("Error: command too long. Buffer overflow prevented.");
            bufferIndex = 0;
        }
    }
}

void CommandManagerService::executeCommand(const char *command)
{
    if (!commandProcessor)
        return;

    if (logger)
    {
        logger->debug("Executing command");
    }

    if (commandProcessor->isValidCommand(command))
    {
        commandProcessor->processCommand(command);
    }
    else
    {
        Serial.println("Invalid command. Type 'help' for available commands.");
    }
}
