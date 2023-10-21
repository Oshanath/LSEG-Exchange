# Installation

1. Recursively clone the repository
```
git clone --recursive https://github.com/Oshanath/LSEG-Exchange.git
```

2. Open the project in Visual Studio and build the solution

3. Check if `TRADER.exe` and `EXCHANGE.exe` are both available as debug targets in Visual Studio. If not, restart Visual Studio and check.

4. Run `EXCHANGE.exe` first and then run `TRADER.exe` in 2 different Visual Studio instances.

# Development

* Select `TRADER.exe` as the target and keep that console running.
* Select `EXCHANGE.exe` as the target to get the data from trader to exchange application.
* If you restart the exchange application, simply click on the running trader console and press enter.