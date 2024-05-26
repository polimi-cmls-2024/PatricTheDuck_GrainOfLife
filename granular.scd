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














