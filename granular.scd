s.boot;

s.reboot;

Server.killAll;

(
s.meter;
s.scope;
)

s.options.device = "MY DEVICE";

s.options.postln;


NetAddr.langPort;



n = NetAddr("localhost", 57120);  // Target address (localhost, default OSC port)

//This function receives 3 arguments relative to:
// msg[1] --> number of alive pixels
// msg[2] --> x-coordinate
// msg[3] --> y-coordinate
// the number of alive pixels is going to affect the overall grain density
// the x coordinate is used to set the pitch  in the range +/- 1 octave
// the y coordinate will increase the grains' duration randomness
(
o = OSCFunc({ arg msg, time, addr, recvPort;
	var pixels = msg[1], coordX = msg[2] , coordY = msg[3];

	g.set(\dens, ((pixels/(100*100))*600));
	g.set(\rate, ((coordX/100)*1.5)+0.5);
	g.set(\durRand, (coordY/100));

}, "/play", a)

)

o.free;

b = Buffer.read(s, "/Users/budger/Desktop/sample/sample2.wav");


b.play;

b.free;
s.clear;



/* GRANULATOR PARAMETERS

1. Number of output channels (default = 2)

2. Time periodicity of the grain arg sync --> is a boolean variable that switch between the Dust and the Impulse function sync = 0 correspond to Dust() --> non periodic repetitons of the grai sync = 1 correspond to Impulse() --> periodic repetitons of the grain
	arg dens --> desity of grains per seconds es. dens = 10 Impulse(dens); --> we have 10 repetiton of the grain in a second

3. Size of the grain arg grainDur --> duration of a grain
	arg durRand --> is a variable that we can use if we want random changes of durations

4. Audio sample arg buf --> has to be = the buffer that contains our audio sample N.B. audio sample has to be mono

5. Playback rate arg rate --> is a variable that can change the pitch of the grain: = 0.5 is an octave lower, = 2 is an octave higher

6. Position of the grain respect the track length arg pos --> has to be a value between 0 and 1, indicates where the grain is taken
	arg posSpeed --> is a variable that changes the speed of the grain
	arg posRans --> is a parameter we can use to random sligthly change the positon of a grain taken for the future repetition

7. Interpolation method (default = 2)

8. Panning arg pan --> is a value between -1 and 1 that determines the spatial position of the sound
	arg panHz --> is the frequency of a possible variation in panning (is at kr)
	arg panRand --> is a value between 0 and 1 that is responsable to panning random variations

9. Shape of the envelope arg grainEnv --> is the shape used for a grain reproduction, you can create a custom one

10. Parameters for general envelope of the sound arg atk arg sus arg rel arg c0 arg c1

11. Max number of overlapping grains (default = 512)

12. Additional parameters arg amp --> is the amplitude of the output signal arg out

*/

(
SynthDef.new(\gs, {
	arg sync=1, dens=20,
	grainDur=0.1, durRand=1,
	buf=0,
	rate=1,
	pos=0, posSpeed=1, posRand=0,
	grainEnv=(-1), atk=1, sus=9999, rel=1, c0=1, c1=(-1),
	pan=0, panHz=0.1, panRand=0,
	amp=1, out=0;

	var sig, env, densCtrl, durCtrl, posCtrl, panCtrl;

	densCtrl = Select.ar(sync, [Dust.ar(dens), Impulse.ar(dens)]);

	durCtrl = grainDur * LFNoise1.kr(100).exprange(1/durRand,durRand);

	posCtrl = Phasor.ar(0, posSpeed * BufRateScale.ir(buf), 0, BufSamples.ir(buf));
	posCtrl = posCtrl + LFNoise1.kr(100).bipolar(posRand*SampleRate.ir);
	posCtrl = posCtrl / BufSamples.ir(buf);
	posCtrl = posCtrl + pos;

	panCtrl = pan + LFNoise1.kr(panHz).bipolar(panRand);

	env = EnvGen.ar(Env.new([0,1,1,0],[atk,sus,rel],[c0,0,c1]),doneAction:2);

	sig = GrainBuf.ar(
		2,
		densCtrl,
		durCtrl,
		buf,
		rate,
		posCtrl,
		2,
		panCtrl,
		grainEnv
	);

	sig = sig * env * amp;

	Out.ar(out, sig);
}).add;
)

(
g = Synth.new(\gs, [
	\buf, b,
	\dens, 10,
	\sync, 1,
	\rate, 1,
	\grainDur, 0.05,
	\posSpeed, 0,
	\pos, 0.65,
	\posRand, 0.7,
	\grainEnv, ~customEnv,
	\panHz, 100,
	\panRand, 1,
]);
)

(
e = Env.new([0,1,0], [0.01,1], [0,-4]);
~customEnv = Buffer.sendCollection(s, e.discretize(8192));
)

s.freeAll;

g.free;














