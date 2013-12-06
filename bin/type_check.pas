


program example(input, output);
var x: integer;
var y: real;
var z: array [1..4] of real;
var a: integer;
function test(a: integer; b: real): real;
begin
	test := 1.01
end;
begin
	x := 102.5;
	y := 123.4;
	x := y;
	z := x > y;
	z[1] := 100;
	z[2] := y;

	y := test(1, 204.3, 3, 4);
	y := test(1);
	y := test(123.4, 100);
	x := test(100, 123.4);
	y := test(100, 100.2)
end.