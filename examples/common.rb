require 'open3'

#Encapsulates an invocation to dirac executable
class DiracInvocation

	# Initializes an invocation object
	#
	# @param [String] info - description string
	# @param [String] expr - expression to be passed to dirac executable with "-e" key
	# @param [Integer] line_length - number of terms per line; to be passed to dirac executable with "-l" key
	# @param [String] lhs - optional left hand side of the formatted result equation. If not provided, expr is substituted
	# @param [String] mode - optional arithmetic mode, to be passed with "-m" key, recognized v alues are 'float' and 'rational'
	# @param [String] dummy - optional dummy index name, to be passed with "-d" key
	# @param [String] apply_symmetry - to be passed with "-s" key; allowed values are "true" and "false" 
	def initialize(info, expr, 
		line_length: 0, lhs: nil,
		mode: nil, dummy: nil, apply_symmetry: nil)
		@info = if !info.nil?
					info
				else
					""
				end 

		@expr = expr

		@line_length = if !line_length.nil?
					   		line_length
					   else
					   		0
					   end 

		@lhs = lhs
		@mode = mode
		@dummy = dummy
		@apply_symmetry = apply_symmetry
	end

	def expr
		@expr
	end

	def line_length
		@line_length
	end

	# Appends stored parameters to executable path as command line options
	#
	# @param [String] executable - path to dirac executable
	# @return command line invocation string made of executable and options
	def cmd(executable)
		res = "#{executable} -e \"#{expr}\""
		if (!@line_length.nil? && (@line_length > 0))
			res += " -l #{line_length}"
		end

		if !@mode.nil?
			res += " -m #{@mode}"
		end

		if !@dummy.nil?
			res += " -d #{@dummy}"
		end

		if !@apply_symmetry.nil?
			res += " -s #{@apply_symmetry}"
		end

		return res
	end

	def info
		@info
	end

	# Invokes dirac executable with arguments stored in the callee 
	# and returns output formatted as LaTeX equation
	#
	# @param executable - dirac executable path
	# @return LaTeX-formatted output
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
