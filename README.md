# WeatherWarningValueChain
Git repository for studying different aspects of thew weather early warning value chain. 


We follow the weather early warning value chain as described by WMO's HiWeather research effort.
___________
The weather early warning value chain:
  1. Observation;
  2. Weather forecast;
  3. Hazard forecast;
  4. Impact forecast;
  5. Warning;
  6. Decision.
See their textbook "Towards the 'perfect' weather warning" edited by Brian Golding (DOI: 10.1007/978-3-030-98989-7)
____________

Specifically we want to look at how design and control adaptive scanning strategies for one meteorological radar in a larger network of radars. We will only look at a single weather phenomenon and only a limited number of impacts to ensure the tractability of the developed code, namely extreme rainfall with impacts on urban flooding, landslides, and agricultural protection. The results of this first work will drive the development of future work. 

Inherent to adaptive control systens is the need for feedback loops. Current (2025) applications of adaptive weather systems optimise the observations on metrics on observations. We want to look further down the chain: can we optimise the observations using metrics on the hazard forecast?

The effects of changes in observations on components further down the chain is also an area where little research exists. By modelling these components with simple models, we hope to shed some light into this aspect. Whilst, for now, we don't want to optimise over the whole value chain (connecting 1 and 6), this can always be done.

 This work, project SMARTER, is funded by the Dutch Research Council (NWO) under the OTP 19959 grant.
