require File.expand_path('common.rb', File.dirname(__FILE__))

dirac = ARGV[0]

HELP = <<-'HELPTEXT'
Usage: ruby fierz6.rb /path/to/dirac/executable
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

forms = [
	["1", "1", "1"],
	["1", "\\gamma_\\mu", "\\gamma^\\mu"],
	["1", "\\sigma_{\\mu\\nu}", "\\sigma^{\\mu\\nu}"],
	["1", "\\gamma5\\gamma_\\mu", "\\gamma5\\gamma^\\mu"],
	["1", "\\gamma5", "\\gamma5"],
	["\\gamma5", "\\gamma_\\mu", "\\gamma5\\gamma^\\mu"],
	["\\gamma_\\mu", "\\gamma_\\nu", "\\sigma^{\\mu\\nu}"],
	["\\gamma5\\gamma_\\mu", "\\gamma5\\gamma_\\nu", "\\sigma^{\\mu\\nu}"],
	["\\epsilon_{\\kappa\\lambda\\mu\\nu}\\gamma^\\kappa", "\\gamma5\\gamma^\\lambda", "\\sigma^{\\mu\\nu}"],
	["\\epsilon_{\\kappa\\lambda\\mu\\nu}\\gamma5", "\\sigma^{\\kappa\\lambda}", "\\sigma^{\\mu\\nu}"],
	["{\\sigma_\\kappa}^\\lambda", "{\\sigma_\\lambda}^\\mu", "{\\sigma_\\mu}^\\kappa"]
]

ins1 = ["1", "\\gamma_\\alpha", "\\sigma_{\\alpha\\beta}", "\\gamma5\\gamma_\\alpha", "\\gamma5"]
ins2 = ["1", "\\gamma_\\gamma", "\\sigma_{\\gamma\\delta}", "\\gamma5\\gamma_\\gamma", "\\gamma5"]

output = File.open("fierz6.tex", "w+")

begin
	output.write(PREAMBLE)

	for iForm in 0...forms.length do
		for m1 in ins1 do
			for m2 in ins2 do
				form = forms[iForm]
				expr = "#{form[0]}*#{m1}*#{form[1]}*#{m2}*#{form[2]}"
				lhs = "#{expr}"
				lhs.gsub!("\\gamma5", "\\gamma^5")

				inv = DiracInvocation.new("", expr, lhs: lhs, line_length: 4)

				puts("Computing #{expr}")

				res = inv.result_latex(dirac)
				output.write("#{res}\n")
			end
		end
	end

	output.write(ENDDOC)
ensure
	output.close()
end

system("pdflatex --halt-on-error fierz6")

exit(0)

