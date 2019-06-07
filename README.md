# Generator-Reason

## Usage

You need Esy, you can install the beta using [npm](https://npmjs.com):

    % npm install -g esy@latest

> NOTE: Make sure `esy --version` returns at least `0.5.4` for this project to build.

Then run the `esy` command from this project root to install and build depenencies.

    % esy

Now you can run the program with `esy x Hello`

## TODO

This is a working list, more todos will probably be needed

* [x] make sure the events are in the correct order
* [x] save the events to a file
* [x] implement a mechanism for different kind of correctness of playing a game
* [x] implement a mechanism for different kind of speed of playing a game
* [ ] implement a mechanism for deciding on when to play a game
* [ ] implement a mechanism for deciding when to create a quiz
* [ ] implement different player types (fast answers, always correct, slow, only play one time, top players, only create a quiz, never play, bots,...)
* [ ] implement random names and questions with answers
* [ ] implement marketing campaign
* [ ] add command line interface to select different scenarios
* [ ] make binaries for linux, mac and windows
* [ ] benchmark: goal stay under 10 seconds for 1milion events

## Extra questions

* Reverse engineer some of the gmae mechanics. For example how long before a game starts, how long before a question timeout?
* changed gameEvent to include the player that opened a game
