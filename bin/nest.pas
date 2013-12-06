program example(input, output);
var x: integer; var y: integer;

function fun1(a: integer; b: integer): integer;

	function fun2(a: integer; b: integer): integer;
		var c: integer;
		var d: real;
		var e: array [1..2] of real;
		var f: integer;

		function fun3(a: integer; b: integer): integer;
			begin
				x := 6;
				y := -3;
				while (x > y) do x := x - 1;
				if (x = -3) then x := x / 3 * 1;
				fun3 := a + b / x
			end;
		
		begin
			x := 2;
			fun2 := fun3(a + x, b mod y)
		end;

	begin
		x := 1;
		y := 3;
		fun1 := fun2(a, b mod x)
	end;

begin
	x := fun1(1, 9)
end.