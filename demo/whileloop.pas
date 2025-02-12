program whileprog;

function fib(n:integer): integer;
begin
	if (n <= 2) then
		fib := 1
	else
		fib := fib(n-1) + fib(n-2);
end;

var
	i: integer;
	x: integer;

begin
	i := 1;
	while(i < 16) do
	begin
		x := fib(i);
	end;
		
end.
