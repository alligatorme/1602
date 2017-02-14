
data=string.char(0xaa,0xb4,0x06,0x01,0xaa,0xc5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xab,0xc5,0xff,0xff,0x05,0xab)
function pm(rsp)
	rsp={string.byte(rsp,1,#rsp)}
	local i=1
	local pm={0,0,0,0}
	local k=0
	local sum=0
	while (i+9<=#rsp)
	do
		print(i)
		if (rsp[i]==170 and rsp[i+1]==192 and rsp[i+9]==171)
		then
			for j=i+2,i+7 do
				sum=sum+rsp[j]
			end
			if ((sum % 256)==rsp[i+8])
			then
				for j=1,4 do
					pm[j]=pm[j]+rsp[i+j+1]
				end
				k=k+1
				i=i+9
			end
		end
		i=i+1
	end
	for j=1,4 do
		pm[j]=math.floor(pm[j]/k)
	end
	return string.char(unpack(pm))
end
print(pm(data))

a={1,3,5,7}
table.remove(a,1)
for i=1,3 do
	print(a[i])
end

rspd={}
function confirm(cfm)
	cfm={string.byte(cfm,1,#cfm)}
	for j=1,#cfm do
		rspd[#rspd+1]=cfm[j]
	end
	while (#rspd>0)
	do
		if (rspd[1]==170 and rspd[2]==197)
		then
			if (#rspd>=10)
			then
				if (rspd[10]==171)
				then
					local rsp={}
					for j=1,10 do
						rsp[#rsp+1]=rspd[1]
						table.remove(rspd,1)
					end
					return unpack(rsp)
				else
					table.remove(rspd,1)
					table.remove(rspd,1)
					print('remove')
				end
			else
				break
			end
		else
			table.remove(rspd,1)
		end
	end
end
confirm(data)
print(confirm(data))
print(unpack(rspd))

function cnfm(cfm)
	cfm={string.byte(cfm,1,#cfm)}
	for j=1,#cfm do
		rspd[#rspd+1]=cfm[j]
	end
	local sum=0
	while (#rspd>0)
	do
		if (rspd[1]==170 and rspd[2]==197)
		then
			if (#rspd>=10)
			then
				if (rspd[10]==171)
				then
					for j=3,8 do
						sum=sum+rspd[j]
					end
					local rsp={}
					for j=1,10 do
						rsp[#rsp+1]=rspd[1]
						table.remove(rspd,1)
					end
					return rsp
				else
					table.remove(rspd,1)
					table.remove(rspd,1)
				end
			else
				break
			end
		else
			table.remove(rspd,1)
		end
	end
end

function cmp(str)
	local tb_rsp={}
	local rsd=string.char(0xaa,0xc5,0x02,0x01,0x00,0x00,0xcc,0x0b,0xda,0xab)
	tb_rsp['Init']=rsd
	rsd=string.char(0xaa,0xc5,0x02,0x01,0x01,0x00,0xcc,0x0b,0xdb,0xab)
	tb_rsp['Passive']=rsd
	rsd=string.char(0xaa,0xc5,0x06,0x01,0x00,0x00,0xcc,0x0b,0xde,0xab)
	tb_rsp['Sleep']=rsd
	rsd=string.char(0xaa,0xc5,0x06,0x01,0x01,0x00,0xcc,0x0b,0xdf,0xab)
	tb_rsp['Measure']=rsd
	for k,v in pairs(tb_rsp) do
		if (v==str)
		then
			return k
		end
	end
end

function gpm(rsp)
	rsp={string.byte(rsp,1,#rsp)}
	local i=1
	local k=0
	local sum=0
	local pm={0,0,0,0}
	while (i+9<=#rsp)
	do
		if (rsp[i]==170 and rsp[i+1]==192 and rsp[i+9]==171)
		then
			for j=i+2,i+7 do
				sum=sum+rsp[j]
			end
			if ((sum % 256)==rsp[i+8])
			then
				for j=1,4 do
					pm[j]=pm[j]+rsp[i+j+1]
				end
				k=k+1
				i=i+9
			end
		end
		i=i+1
	end

	for j=1,4 do
		pm[j]=math.floor(pm[j]/k)
		if (pm[j]==0x00)
		then
			pm[j]=0x0f
			pm[#pm+1]=0xaa
		else
			pm[#pm+1]=0xee
		end
	end
	return string.char(unpack(pm))
end


			if (trgr==6)
			then
				local rd
				rd=cnfm(data,trgr)
				if (rd)
				then
					rd=string.char(unpack(rd))
					if tb_rsp[rd]
					then
						conn:send(tb_rsp[rd])
--						trgr=0
					else
						conn:send('Fail index')
					end
--				else
--					conn:send('No respose!')
				end
			end


	function s_output(str)
		if(conn~=nil) then
			conn:send(conn:getpeer()..":"..str.."\n")
		end
	end
	node.output(s_output, 1)
	local tb_rsp={
		[string.char(0x02,0x01,0x00,0x00,0xb4,0x6f)]='Init',
		[string.char(0x02,0x01,0x01,0x00,0xb4,0x6f)]='Passive',
		[string.char(0x06,0x01,0x00,0x00,0xb4,0x6f)]='Sleep',
		[string.char(0x06,0x01,0x01,0x00,0xb4,0x6f)]='Measure',
	}
