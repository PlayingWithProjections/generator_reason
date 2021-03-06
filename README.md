# Generator-Reason

## Usage

You need Esy, you can install the beta using [npm](https://npmjs.com):

    % npm install -g esy@latest

> NOTE: Make sure `esy --version` returns at least `0.5.4` for this project to build.

Then run the `esy` command from this project root to install and build depenencies.

    % esy

Now you can run the program with `esy x Generator`

## TODO

refactorings needed

* [ ] add rei files for everything
* [ ] discover better names for the distributions
* [ ] make the month distribution mutable so that distribution is responsible for changing it

This is a working list, more todos will probably be needed

* [x] make sure the events are in the correct order
* [x] save the events to a file
* [x] implement a mechanism for different kind of correctness of playing a game
* [x] implement a mechanism for different kind of speed of playing a game
* [x] implement a mechanism for deciding on when to play a game
* [x] implement a mechanism for deciding when to create a quiz
* [x] implement random names and questions with answers
* [ ] implement different player types (fast answers, always correct, slow, only play one time, top players, only create a quiz, never play, bots,...)
* [ ] implement boring and popular quizzes
* [ ] implement marketing campaign
* [ ] implement different kind of players: playing many games, creating many quizzes
* [ ] add command line interface to select different scenarios (is this necessary?)
* [x] make binaries for linux, mac and windows
* [x] benchmark: goal stay under 10 seconds for 1 million events (1152929 events = 5.5 seconds currently so we're well under our goal)
* [x] extract benchmarks to a different json deps so I can keep building them but they don't hinder the build pipeline

## Extra questions

* Reverse engineer some of the gmae mechanics. For example how long before a game starts, how long before a question timeout?
* changed gameEvent to include the player that opened a game

## Benchmark

Current results show that we have 2 slow parts:
1. The printing of a datetime to string (https://github.com/ocaml-community/calendar/issues/24)
2. The writing to json.

Hopefully 1 is fixable. 2 will probably not be easily fixable.
If we can fix 1, then we can probably land on 1milj events in about 15seconds which should be ok

We're able to fix 1 a bit with using ptime. It might be possible to go even faster with `Core.date`.
For example, now with 20 days a full run takes about `566ms`, without to json `210ms` and without outputting the events `40ms`

After benchmarking and searching, I found that using `core` is not possible on windows. Getting help from [Discuss.ocaml](https://discuss.ocaml.org/t/performance-of-printf-sprintf/3936/5) I found an implementation that is even 4 times faster than core. Awesome

### Fixing events to json with core unix time

┌─────────────────────────┬────────────┬─────────┬──────────┬──────────┬────────────┐
│ Name                    │   Time/Run │ mWd/Run │ mjWd/Run │ Prom/Run │ Percentage │
├─────────────────────────┼────────────┼─────────┼──────────┼──────────┼────────────┤
│ from_unixfloat calendar │   201.35ns │  47.00w │          │          │      6.53% │
│ from_unixfloat ptime    │    40.05ns │  19.00w │          │          │      1.30% │
│ from_unixfloat core     │    29.97ns │  10.00w │          │          │      0.97% │
│ format calendar         │ 3_082.18ns │ 721.26w │    1.07w │    1.07w │    100.00% │
│ format ptime            │   963.04ns │ 215.01w │          │          │     31.25% │
│ format core             │   227.84ns │   5.00w │          │          │      7.39% │
└─────────────────────────┴────────────┴─────────┴──────────┴──────────┴────────────┘

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

#### Total with core

┌────────────┬──────────┬─────────┬──────────┬──────────┬────────────┐
│ Name       │ Time/Run │ mWd/Run │ mjWd/Run │ Prom/Run │ Percentage │
├────────────┼──────────┼─────────┼──────────┼──────────┼────────────┤
│ simulation │  47.60ms │ 11.88Mw │ 812.76kw │ 812.76kw │    100.00% │
└────────────┴──────────┴─────────┴──────────┴──────────┴────────────┘

4 runs => 46.44ms to 47.60ms

*with to events*

┌────────────┬──────────┬─────────┬──────────┬──────────┬────────────┐
│ Name       │ Time/Run │ mWd/Run │ mjWd/Run │ Prom/Run │ Percentage │
├────────────┼──────────┼─────────┼──────────┼──────────┼────────────┤
│ simulation │ 177.99ms │ 19.64Mw │   6.07Mw │   6.07Mw │    100.00% │
└────────────┴──────────┴─────────┴──────────┴──────────┴────────────┘

*full run*

┌────────────┬──────────┬─────────┬──────────┬──────────┬────────────┐
│ Name       │ Time/Run │ mWd/Run │ mjWd/Run │ Prom/Run │ Percentage │
├────────────┼──────────┼─────────┼──────────┼──────────┼────────────┤
│ simulation │ 250.85ms │ 20.04Mw │   7.27Mw │   5.55Mw │    100.00% │
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

#### with core date

including to json = 18.27s (huge improvement)
