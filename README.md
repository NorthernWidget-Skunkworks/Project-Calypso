# Project-Calypso
The hardware and software designs for a turbidity measurement system, based on the work of a 2018 UMN senior design team

## Project Summary

Our team developed an open-source sensor to measure water turbidity. Turbidity, or "murkiness", increases when the water carries more sediment. Dissolved compounds can also increase turbidity by reducing water clarity. In general, less suspended and dissolved material makes better drinking water. Anyone may download and build this sensor (Design: https://doi.org/10.5281/zenodo.8177929).

<img src="https://github.com/NorthernWidget-Skunkworks/Project-Calypso/blob/master/Documentation/images/Calypso_LED_and_detector_schematic.png" alt="UMN WRC" width="400px">

*Two LEDs, red and infrared, send beams across the water to be measured, which lies within a transparent tube. These LED beams interact with the water, which reduces transmission and increases sidewas scatter. The detectors record both changes.*

The sensor, called "Project Calypso", shines lights from two LEDs through the water. These LEDs lie at right angles from each other along a clear square plastic tube. Detectors record the LEDs' brightness. The sensor directly across from each LED records light transmission. The sensor at 90 degrees from each LED records light scattering. Sediment in the water blocks the light and reduces transmission. Light can reflect off of sediment and scatter onto the sensor at 90 degrees. The scattering pattern depends on the wavelength of the light and sediment size. Therefore, we use both visible (red) and infrared light to learn about sediment size. Dissolved material reduces light transmission but does not scatter the light sideways. Because of this, Project Calypso can differentiate between sediment and dissolved compounds.

## Funding Support

Funding for this work came from the ***University of Minnesota Water Resources Center***, with support from the **USGS WRRI** program. This funding supported a grant to Wickert, Sandell, and Karwan, for their proposal entitled, *Open-Source Turbidity Sensor to Monitor Suspended and Dissolved Matter in Surface Water*.

<img src="https://github.com/NorthernWidget-Skunkworks/Project-Calypso/blob/master/Documentation/images/UMN_WRC_400x400.jpg" alt="UMN WRC" width="240px">

<br/>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
