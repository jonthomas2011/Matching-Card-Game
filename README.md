Matching Card Game - Final Project
Junior Electrical Design
Jonathan Thomas
------------------------------------

Introduction:

	The goal of this design was to meet all the specification requirements for the Final Project. It had to have a Digital input/output, Analog input/output, use SPI, drive a low impedance load, and use the ADC. 
	My design was to make a simple card matching game in which 3 pairs of letters would be matched.

Design:  

	This game had 3 pairs of letters: 'U', 'F', and 'L' that would have a randomized order each time the game was played. The user would then select which card they would like to play using a 4x4 keypad (specifically the key values of 4-9). When a key would be pressed, its corresponding card would be flipped. Once the second choice was made it would compare the first choice to the second choice. If the character values of the first and second choice matched both cards would stay flipped and the game would continue. If they didn’t match, both cards would turn back into “#” value which was the default character representation of an un-flipped card.
	Each time a key was pressed a yellow LED would blink along with a specific frequency being played out of the speaker. If a match was made, a higher frequency would be output and a green LED would blink. If no match, a lower frequency and a red LED.

	
Components Included:

	Microcontroller		Yes - TI MSP430
	Digital Input		Yes – Keypad presses
	Digital Output		Yes – lights flashing at end of game
	Analog Input		Yes – alters speed at which lights blink
	Analog Output		Yes – music plays at end of game
	Speaker				Yes
	SPI					Yes – connected to DAC which connects to speaker
	DAC					Yes
	LCD status			Yes – displays 8 cards on screen and count of moves
