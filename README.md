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

## Benchmark

Ok, so we best keep benchmarking my program, but the overal speed is ok.
The problem is in the serialization. That's 10times slower.
(also it's not the writing tot the file I think)
Options to look at: 
1. benchmark the toJson and see if we can get something out of that.
2. merge the to_file and toJson in one step and use a stream_to_file?


### Without to json and 10 days

The translating the events json and writing takes much longer than generating the events. 40-50ms for converting to jsonEvents and the same amount of time to write to a file.
these are between 7000-12000 events. Console logging adds time (4ms)

┌────────────┬──────────┬─────────┬──────────┬──────────┬────────────┐
│ Name       │ Time/Run │ mWd/Run │ mjWd/Run │ Prom/Run │ Percentage │
├────────────┼──────────┼─────────┼──────────┼──────────┼────────────┤
│ simulation │   7.37ms │  2.53Mw │ 121.00kw │ 121.00kw │    100.00% │
└────────────┴──────────┴─────────┴──────────┴──────────┴────────────┘

### Without to json and 20 days

these are between 45000-80000 events. Console logging adds time (4ms)
about 6 times more => about 6 times slower.
Writing them out to json takes about 1s total
Converting them to json takes about 350ms total.

┌────────────┬──────────┬─────────┬──────────┬──────────┬────────────┐
│ Name       │ Time/Run │ mWd/Run │ mjWd/Run │ Prom/Run │ Percentage │
├────────────┼──────────┼─────────┼──────────┼──────────┼────────────┤
│ simulation │  43.95ms │ 12.13Mw │ 843.93kw │ 843.93kw │    100.00% │
└────────────┴──────────┴─────────┴──────────┴──────────┴────────────┘

### Without to json and 100 days

4346368 events
scales linear

To json = 42 secs (to be clear, it's actually the tojson and not the rev_map that takes time)
To json + file = 88secs
The file is then 1.4G big.

┌────────────┬──────────┬──────────┬──────────┬──────────┬────────────┐
│ Name       │ Time/Run │  mWd/Run │ mjWd/Run │ Prom/Run │ Percentage │
├────────────┼──────────┼──────────┼──────────┼──────────┼────────────┤
│ simulation │    3.15s │ 533.31Mw │  90.25Mw │  90.25Mw │    100.00% │
└────────────┴──────────┴──────────┴──────────┴──────────┴────────────┘
