--ssid="bjcj-wifi1"
--pswd="bjcjy123456"

ssid="eel"
pswd="102672102672"
sleep=string.char(0xaa,0xb4,0x06,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x05,0xab)
wakeup=string.char(0xaa,0xb4,0x06,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x06,0xab)
passive=string.char(0xaa,0xb4,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x02,0xab)
active=string.char(0xaa,0xb4,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x01,0xab)

wifi.setmode(wifi.STATION)
wifi.sta.config(ssid,pswd)
srv=net.createServer(net.TCP,30)
print(wifi.sta.getip())
gpio.mode(0,gpio.OUTPUT)
gpio.mode(4,gpio.OUTPUT)
gpio.write(0,gpio.HIGH)
gpio.write(4,gpio.HIGH)
n=4
local rspd={}
uart.setup(0,9600,8,0,1,1)

srv:listen(80,function(conn)
	conn:on("receive",function(sck,pld)
		local rdp
		local k
		local trgr=0

		gpio.write(0,gpio.LOW)
		pld=string.gsub(pld,"%s","")


		if (pld=='1')
		then
			rdp={0,0,0,0}
			k=0
			trgr=1
			uart.on("data")
			uart.on("data",10,
			function(data)
				if (trgr==1)
				then
					rd=cnfm(data,trgr)
					if (rd)
					then
						for j=1,4 do
							rdp[j]=rdp[j]+rd[j]
						end
						k=k+1
						if (k==n)
						then
							for j=1,4 do
								rdp[j]=math.floor(rdp[j]/k)
								if (rdp[j]==0x00)
								then
									rdp[j]=0x0f
									rdp[#rdp+1]=0xaa
								else
									rdp[#rdp+1]=0xee
								end
							end
							conn:send(string.char(unpack(rdp)))
							trgr=0
							uart.on("data")
						end
					end
				end
			end,0)
		end

--		uart.on("data",10,
--		function(data)
--			if (trgr==6)
--			then
--				local rd
--				rd=cnfm(data,trgr)
--				if (rd)
--				then
--					rd=tb_rsp[string.char(unpack(rd))]
--					conn:send(rd or 'Fail index')
--				else
--					conn:send('No respose!')
--				end
--			end
--		end,0)

		if (pld=='2')
		then
			gpio.write(4,0)
			conn:send('Close')
		end

		if (pld=='3')
		then
			gpio.write(4,1)
			conn:send('Open')
		end

		if (pld=='4')
		then
			if gpio.read(4)==0
			then
				conn:send('Close')
			else
				conn:send('Open')
			end
		end

		if (pld=='5')
		then
			trgr=6
			uart.write(0,active)
			uart.write(0,sleep)
--			trgr=0
		end

		if (pld=='6')
		then
			trgr=6
			uart.write(0,wakeup)
--			uartwrite(0,active)
--			trgr=0
		end
		if (pld=='7')
		then
			trgr=6
			uart.write(0,active)
		end
		if (pld=='8')
		then
			trgr=6
			uart.write(0,passive)
		end

		gpio.write(0,gpio.HIGH)
	end)
end)

function cnfm(cfm,tp)
	cfm={string.byte(cfm,1,#cfm)}
	if (#rspd>60)
	then
		rspd={}
	end
	for j=1,#cfm do
		rspd[#rspd+1]=cfm[j]
	end
	local sum=0
	while (#rspd>=10)
	do
		if (rspd[1]==170 and rspd[2]==(191+tp) and rspd[10]==171)
		then
			for j=3,8 do
				sum=sum+rspd[j]
			end
			if ((sum % 256)==rspd[9])
			then
				table.remove(rspd,1)
				table.remove(rspd,1)
				local rsp={}
				for j=1,6 do
					rsp[j]=rspd[1]
					table.remove(rspd,1)
				end
				table.remove(rspd,1)
				table.remove(rspd,1)
				return rsp
			end
		end
		table.remove(rspd,1)
	end
	return nil
end
