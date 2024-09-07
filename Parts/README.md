# Parts list
This document shows everything needed to build the leg, for the assembly please read the [Assembly Guide](https://github.com/Faizanfaiz/UWE-Mechatronics/tree/main/Assembly#assembly). 
## 3D prints
>[!NOTE]
>All parts can fit on a minimum printer bed size of 256x256mm.  
>Everything was printed on the Bambu Lab A1 and X1 Carbon in PolyTerra PLA Fossil Grey.
### Print settings
- 35% infill
- Triangular infill
- Supports enabled
- 2 or 3 layer thick walls
- Bottom Z distance = 0.08mm
- Outer and inner brim 5mm

### 
| Thumbnail | Name | Quantity |
|:-----------:|:------:|:----------:|
| <img src="./Images/upper%20leg%20back.png" width=70%> | upper leg back | 1 |
| <img src="./Images/upper%20leg%20front.png" width=70%> | upper leg front | 1 |
| <img src="./Images/lower%20leg%20back.png" width=70%> | lower leg back | 1 |
| <img src="./Images/lower%20leg%20front.png" width=70%> | lower leg front | 1 |
| <img src="./Images/tension%20pin.png" width=70%> | tension pin | 4 |
| <img src="./Images/pulley%20pin.png" width=70%> | pulley pin | 1 |
| <img src="./Images/pulley%20adapter.png" width=70%> | pulley adapter | 1 |
| <img src="./Images/coupler.png" width=70%> | coupler | 1 |
| <img src="./Images/motor-plate.png" width=70%>| motor plate | 1 |

### Depreciated
| Thumbnail | Name | Reason |
|:-----------:|:------:|:----------:|
|  | | | |

## Hardware
### Electronics needed
>[!NOTE]
>The motor can be swapped for similar motors with same diameter and bolting layout.

|  Item   |       Short description      |    Quantity       | Cost per item |
|---------|----------------------|:-------------------:|:---------------:|
| RMD-X8 motor | drive the upper and lower leg | 2 | ~£350 |
| Raspberry pi 5 8GB| drive the motors via can bus and executing code | 1 | ~£80 |
| waveshare 2CH CAN FD HAT | SPI to CAN conversion | 1 | £35.75 |
| SD card 32GB | raspeberry pi os 64x installed | 1 | £8.50 |

### Hardware for the leg
|  Item   |       Short description      |    Quantity       | Cost per item |
|---------|----------------------|:-------------------:|:---------------:|
| M4 inserts | one side of both upper and lower leg | 14 |
| M4x30 cap | pulley pin 					   | 1 |
| M4x20 cap | upper leg enclosure 	   | 8 |
| M4x16 cap | lower leg enclosure	    | 6 |
| M4x8 cap | coupler between both motors and the leg | 11 |
| M4x10 countersunk| attaching driven pulley to lower leg| 8 |
| M5x35 pan head| pulley tensioner	   | 2 |
| M5 nut | pulley tensioner 				  | 2 |
| M4 nut | pulley pin 							  | 1 |
| 26-5M-15 Timing Pulley | moving the lower leg | 2 |
| 635-5M-15 Timing Belt | moving the lower leg  | 1 |
| 22x8x7mm bearing | for the pulleys | 3 |
| 12x8x3.5mm bearing | for the belt tensioner | 4 |
### Hardware for the frame
| Item | notes | Quantity | cost per item|
|------|-------|----------|------|
| 2020 extrusion 1.2m | cut down to length | 3 | £9.22 |
| SBR12 linear guide rail 500mm 2pcs| | 2 | £26.59 |
