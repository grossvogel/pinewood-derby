require 'rubygems'
require 'websocket-gui'
require 'rubyserial'
require 'json'

class App < WebsocketGui::Base

	tick_interval 1.0
	
	on_tick do |connected|
		update_socket_state(connected)
		update_serial_state
		read_serial
	end

	on_start_click do |params|
		write_serial("0")
	end

	on_stop_click do |params|
		write_serial("0")
	end

	def initialize(options = {})
		super
		@serial_buffer = ''
	end

	def update_socket_state(connected)
		send({
			type: 'socket_state',
			state: connected ? 'connected':'disconnected'
		})
	end

	def update_serial_state
		send({
			type: 'serial_state',
			state: serial_connect ? 'connected':'disconnected'
		})
	end

	def update_race_state(state)
		send({
			type: 'race_state',
			state: state
		})
	end

	def record_finish(place, lane, time)
		if place == "1"
			@winning_time = time.to_f
		end

		send({
			type: 'finish',
			lane: lane,
			time: time,
			diff: time.to_f - @winning_time,
			place: place,
		})
	end

	def debug(line)
		send({
			type: 'debug',
			message: line
		})
	end

	def send(data)
		socket_send(data.to_json)
	end

	def read_serial
		serial = serial_connect
		if serial
			while true do
				current = serial.read(1024)
				if current.empty?
					break
				else
					@serial_buffer += current
				end
			end
			parse_serial
		end
	end

	def write_serial(s)
		serial = serial_connect
		if serial
			serial.write s
		end
	end

	#	@TODO: Detect if each line is complete
	#	If not, leave it on the buffer 'til the rest comes in
	#	We may need a custom message delimiter rather than EOL
	def parse_serial
		if (@serial_buffer)
			lines = @serial_buffer.split(/\r?\n\r?/)
			lines.each do |line|
				fields = line.split(':')
				case fields[0]
					when 'state'
						update_race_state(fields[1])
					when 'finish'
						record_finish(fields[1], fields[2], fields[3])
					else
						debug(line)
				end
			end
			@serial_buffer = ''
		end
	end

	private

	def serial_connect
		begin
			Serial.new '/dev/ttyACM0', 9600
		rescue Exception => e
			nil
		end
	end
end

class WebsocketGui::SinatraWrapper
	set :public_folder, File.dirname(__FILE__) + '/assets'
end

app = App.new http_port: 3000, http_host: '127.0.0.1'
app.run! socket_port: 7777, socket_host: '127.0.0.1'

