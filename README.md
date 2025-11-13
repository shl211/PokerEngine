# Poker Engine

This project attempts to provide high performance analysis tools for a variety of poker variants.

## Features

- ⚙️ **Multi-variant support** (in progress)
  - [x] No Limit Hold’em (NLH)
  - [ ] Pot Limit Omaha (PLO)
  - [ ] Short Deck Hold’em (6+ Hold’em)
  - [ ] Seven-Card Stud
- 🎲 **Monte Carlo simulation engine** for flexible equity estimation  
- 🧮 **Exact enumeration** for small state spaces  
- 🃏 **Support for:**
  - Hand vs Hand  
  - Hand vs Range  
  - Range vs Range  
  - Pre-flop and post-flop configurations
- 📈 **Customizable simulation parameters** (e.g. number of trials)

## Installation

Clone the repository:

```bash
git clone git@github.com:shl211/PokerEngine.git
cd PokerEngine
```

Build from source (Linux):

```
mkdir build && cd build
cmake ..
make
```

Run tests to verify installation:

```
ctest
```

Generate documentation:

```
cd docs
doxygen
```

# Usage

Example commands:

```
# Hand vs Hand (NLH pre-flop)
./poker-equity --hero AdKd --villain AhKh #Hand vs Hand pre-flop
```

```
# Hand vs Hand (post-flop)
./poker-equity --game nlh --hero AdKd --villain AhKh --board 2h3cTs
```

```
 #Hand vs Range
./poker-equity --hero AdKd --villain-range "KK+,AKs"
```

```
# Hand vs Range
./poker-equity --game nlh --hero AdKd --villain-range "KK+,AKs"
```

```
# Range vs Range
./poker-equity --game nlh --hero-range "AA,AK,AQ" --villain-range "KK+,AKs"
```

Standard poker notation for specifying cards and ranges are used.