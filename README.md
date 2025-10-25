# Poker Engine

This project provides analysis tools for poker. This currently includes:

- Monte Carlo NLH(No Limit Hold'em) Equity Calculator

# Installation

To build this project from source, clone this repository:

```
git clone git@github.com:shl211/PokerEngine.git
```

To build this project on Linux:

```
mkdir build
cd build
cmake ..
make
```

Check installation by running tests with `ctest`. The application executable is `app`. 
To generate the documentation, navigate to `docs/` and run `doxygen`.

# Monte Carlo NLH Equity Calculator

An NLH equity calculator based off Monte Carlo simulations is implemented for hand-vs-hand, hand-vs-range, range-vs-hand, and range-vs-range. Pre-flop and post-flop situations can all be specified. 

Some examples:

```
./app --hero AdKd --villain AhKh #Hand vs Hand pre-flop
```

```
./app --hero AdKd --villain AhKh --board 2h3cTs #Hand vs Hand post-flop
```

```
./app --hero AdKd --villain-range "KK+,AKs" #Hand vs Range
```

```
./app --hero-range "KK+,AKs" --villain 8d9d  #Hand vs Range
```

```
./app --hero-range "AA,AK,AQ" --villain-range "KK+,AKs" #Range vs Range
```

Standard poker notation for specifying cards and ranges are used.