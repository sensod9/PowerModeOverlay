# PowerModeOverlay
> Recreation of the power mode from [FNaCEC:R](https://gamejolt.com/games/fnacecr/563650) (Five Night's at Freddy's fangame). This customizable overlay let you play any FNaF-like game with power mode

## How to use:
<ol>
  <li><b>Download and run <code>PowerModeOverlay.exe</code> from <a href="https://github.com/sensod9/PowerModeOverlay/releases">releases page</a></b></li>
  <li><b>Enable overlay</b><br />&nbsp;&nbsp;(<b>'F6'</b> by default)</li>
  <li><b>Start a new night</b> in the game</li>
  <li><b>Enable power mode</b><br />&nbsp;&nbsp;(<b>'R'</b> by default because <b>'R'</b> is the most common key to restart night, so power mode syncs when you restart night)</li>
</ol>
<ul>
  <li><b>Toggle visibility</b> when you need to see smth<br />&nbsp;&nbsp;(<b>'Space'</b> by default)</li>
  <li><b>Restart power mode by pressing the button that enables it if you've died</b><br />&nbsp;&nbsp;(<b>'R'</b> by default)</li>
  <li><b>Press the button that enable the overlay if you've finished the night</b><br />&nbsp;&nbsp;(<b>'F6'</b> by default)</li>
</ul>
<ol>
5. <b>Shut down</b> overlay when you've done<br />&nbsp;&nbsp;(<b>'F8'</b> by default)
</ol>

## How to customize:
### You can change **<ins>keybinds</ins>**, **<ins>power amount</ins>**, **<ins>power consumption rate</ins>** and make **<ins>hold "toggle" button to consume power</ins>**  by specifying **<ins>key:value</ins>** pairs in `pmode.cfg` file.
#### The default `pmode.cfg` file comes with the `PowerModeOverlay.exe` on every release. Also you can create it yourself
#### `pmode.cfg` must be in the same folder as `PowerModeOverlay.exe` so you may find useful to have copies of the `PowerModeOverlay.exe` with various `pmode.cfg` files in different games' folders
## Example of `pmode.cfg`:
```
[KEYBINDS]
enable: F2
restart: R
toggle: Shift
shutdown: default           ; F6  (it's just comment)

[POWER_SETTINGS]
amount: 199     	        ; 199%
consumption: 15 	        ; 1 = 1ms to consume 0.1% of power. 
                 	        ; on consumption=50 it takes 15*100*10ms = 15000ms = 15s
               	 	        ; to consume 100% if the vision is uncovered

[ADDITIONAL_FLAGS]
holdToConsumePower: true 	; makes hold "toggle" button to consume power

[POSITIONING]
x: 99
y: 9
```
### Default values reference and keybinds reference are in `pmode.cfg` that comes on every [release](https://github.com/sensod9/PowerModeOverlay/releases)
