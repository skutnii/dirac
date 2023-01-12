require 'open3'

dirac = ARGV[0]

HELP = <<-'HELPTEXT'
Usage: ruby tests.rb /path/to/dirac/executable
HELPTEXT

if dirac.nil?
	print HELP
	exit(1)
end

dirac = File.expand_path(dirac, Dir.pwd)

PREAMBLE = <<-'TEXT'
\documentclass[aps,prd,a4paper]{revtex4-2}
\usepackage[T1]{fontenc}
\usepackage{underscore}
\usepackage{amsmath}
\usepackage{amssymb}

\begin{document}
TEXT

ENDDOC = "\\end{document}"

outName = ARGV[1]
if outName.nil? 
	outName = "tests"
end

shortName = outName
if !outName.end_with? ".tex"
	outName = outName + ".tex"
else
	shortName = outName.gsub(".tex", "")
end

class DiracTest
	def initialize(info, expr, line_length: 0, lhs: nil)
		@info = info 
		@expr = expr

		@line_length = line_length
		if @line_length.nil?
			@line_length = 0
		end 

		@lhs = lhs
	end

	def expr
		@expr
	end

	def line_length
		@line_length
	end

	def cmd(executable)
		res = "#{executable} -e \"#{expr}\""
		if @line_length > 0
			res += " -l #{line_length}"
		end

		return res
	end

	def info
		@info
	end

	def result_latex(executable)
		command = self.cmd(executable)

		processed, ex_code = Open3.capture2(command)
		if ex_code != 0
			processed = "\\verb|#{processed.rstrip}|"
		end 

		eqn = if !@lhs.nil?
				@lhs
			  else
			  	@expr
			  end

		eqn = "#{eqn} = #{processed.rstrip}"
		if (@line_length > 0) && (ex_code == 0)
			eqn = "\\begin{split}\n#{eqn}\n\\end{split}"
		end
		eqn = "\\begin{equation}\n#{eqn}\n\\end{equation}"
		return eqn 
	end
end

TESTS = [
	{:info => "Invalid input", :expr => "foo"},

	{:info => "Kronecker trace", :expr => "\\delta_\\mu^\\mu"},
	
	{:info => "Product of Levi-Civita symbols",
			:expr => "\\epsilon_{\\alpha\\beta\\gamma\\delta}\\epsilon_{\\kappa\\lambda\\mu\\nu}",
			:line_length => 5},

	{:info => "Product of Levi-Civita symbols with a contraction",
			:expr => "\\epsilon_{\\alpha\\beta\\gamma\\delta}{\\epsilon^\\delta}_{\\lambda\\mu\\nu}",
			:line_length => 3},

	{:info => "Product of Levi-Civita symbols with two contractions",
			:expr => "\\epsilon_{\\alpha\\beta\\gamma\\delta}{\\epsilon^{\\delta\\gamma}}_{\\mu\\nu}",
			:line_length => 3},

	{:info => "Product of two gamma matrices", 
			:expr => "\\gamma^\\mu\\gamma^\\nu"},

	{:info => "Product of $\\gamma$ and $\\gamma^5$", 
			:expr => "\\gamma^\\mu\\gamma5",
			:lhs => "\\gamma^\\mu\\gamma^5"},

	{:info => "$\\gamma^5$ sandwiched between two $\\gamma$", 
			:expr => "\\gamma^\\mu\\gamma5\\gamma^\\nu",
			:lhs => "\\gamma^\\mu\\gamma^5\\gamma^\\nu"},

	{:info => "Product of three gamma matrices",
			:expr => "\\gamma^\\lambda\\gamma^\\mu\\gamma^\\nu"},

	{:info => "Product of four gamma matrices",
			:expr => "\\gamma^\\kappa\\gamma^\\lambda\\gamma^\\mu\\gamma^\\nu",
			:line_length => 6},

	{:info => "One-gamma \"sandwich\"",
			:expr => "\\gamma_\\lambda\\gamma^\\mu\\gamma^\\lambda"},

	{:info => "Two-gamma \"sandwich\"",
			:expr => "\\gamma_\\lambda\\gamma^\\mu\\gamma^\\nu\\gamma^\\lambda"},

	{:info => "$\\sigma$-matrix multiplied by $\\gamma$",
			:expr => "\\sigma^{\\lambda\\mu}\\gamma^\\nu"},

	{:info => "Same as the above but with $\\sigma$ expanded",
			:expr => "\\I / 2 {\\gamma^\\lambda\\gamma^\\mu - \\gamma^\\mu\\gamma^\\lambda}\\gamma^\\nu",
			:lhs => "\\frac{I}{2}\\left[\\gamma^\\lambda\\gamma^\\mu - \\gamma^\\mu\\gamma^\\lambda\\right]\\gamma^\\nu"},

	{:info => "Product of two $\\sigma$-matrices",
			:expr => "\\sigma^{\\kappa\\lambda}\\sigma^{\\mu\\nu}",
			:line_length => 5},

	{:info => "Product of two expanded $\\sigma$-matrices",
			:expr => "- 1 / 4 {\\gamma^\\kappa\\gamma^\\lambda - \\gamma^\\lambda\\gamma^\\kappa}"\
						"{\\gamma^\\mu\\gamma^\\nu - \\gamma^\\nu\\gamma^\\mu}",
			:lhs => "- \\frac{1}{4}\\left[\\gamma^\\kappa\\gamma^\\lambda - \\gamma^\\lambda\\gamma^\\kappa\\right]"\
						"\\left[\\gamma^\\mu\\gamma^\\nu - \\gamma^\\nu\\gamma^\\mu\\right]",
			:line_length => 5},

	{:info => "$\\sigma$-matrix multiplied by $\\gamma^5\\gamma^\\nu$",
			:expr => "\\sigma^{\\lambda\\mu}\\gamma5\\gamma^\\nu",
			:lhs => "\\sigma^{\\lambda\\mu}\\gamma^5\\gamma^\\nu"},

	{:info => "Expanded $\\sigma$-matrix multiplied by $\\gamma^5\\gamma^\\nu$",
			:expr => "\\I / 2 {\\gamma^\\lambda\\gamma^\\mu - \\gamma^\\mu\\gamma^\\lambda}\\gamma5\\gamma^\\nu",
			:lhs => "\\frac{I}{2}\\left[\\gamma^\\lambda\\gamma^\\mu - \\gamma^\\mu\\gamma^\\lambda\\right]\\gamma^5\\gamma^\\nu"},

	{:info => "Product of $\\sigma$ and $\\gamma^5$ matrices",
			:expr => "\\sigma^{\\lambda\\mu}\\gamma5",
			:lhs => "\\sigma^{\\lambda\\mu}\\gamma^5"},

	{:info => "Same as above in the reverse order",
			:expr => "\\gamma5\\sigma^{\\lambda\\mu}",
			:lhs => "\\gamma^5\\sigma^{\\lambda\\mu}"},
]

output = File.open(outName, "w+")

begin
	output.write(PREAMBLE)

	for item in TESTS
		test_case = DiracTest.new(item[:info], 
					item[:expr], 
					line_length: item[:line_length],
					lhs: item[:lhs])
		res = test_case.result_latex(dirac)
		output.write("#{test_case.info}\n#{res}\n\n")			 
	end
	
	output.write(ENDDOC)
ensure
	output.close()
end

pdflatex = "pdflatex -halt-on-error #{shortName}"
print "Running #{pdflatex}...\n" 
	
system(pdflatex)

exit(0)