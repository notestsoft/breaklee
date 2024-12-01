$Processes = @{}
$NotificationUrl = ""

function Send-Notification {
	param (
		[string]$Message
	)
	
	$Headers = { }
	$Payload = [PSCustomObject]@{
		content = $Message
		embeds = @[
			[PSCustomObject]@{
				title = "Server Status"
			}
		]
	}
	
	#Invoke-RestMethod -Uri $global:NotificationUrl -Method Post -Headers $Headers -Body ($Payload | ConvertTo-Json) -ContentType 'application/json'
}

function Add-Process {
	param (
		[string]$Name,
		[string]$Path,
		[string[]]$Arguments
	)
	
	$ProcessInfo = [PSCustomObject]@{
        Name      = $Name
		Path 	  = $Path
        Arguments = $Arguments
    }
    
	$global:Processes[$Name] = $ProcessInfo
}

function Monitor-Process {
	param (
		[string]$Name
	)
	
	$ProcessInfo = $global:Processes[$Name]
	$Running = Get-Process $ProcessInfo.Name -ErrorAction SilentlyContinue
	
	if (-not $Running) {
        if ($ProcessInfo.Arguments -ne $null) {
            Start-Process -FilePath $ProcessInfo.Path -ArgumentList $ProcessInfo.Arguments -PassThru -ErrorAction SilentlyContinue
        } else {
            Start-Process -FilePath $ProcessInfo.Path -PassThru -ErrorAction SilentlyContinue
        }

		Send-Notification -Message 'Restarting Server {0} ...' -f @($ProcessInfo.Name)
    }
}

function Remove-Process {
	param (
		[string]$Name
	)
	
	$ProcessInfo = $global:Processes[$Name]
	Get-Process $ProcessInfo.Name -ErrorAction SilentlyContinue | Stop-Process -Force
}

function Start-Monitoring {
	$Monitoring = $true
	while ($Monitoring) {
		foreach ($Name in $global:Processes.Keys) {
			Monitor-Process -Name $Name
		}
		
		Start-Sleep -Seconds 1
	}
}

function Cleanup {
	foreach ($Name in $global:Processes.Keys) {
		Remove-Process -Name $Name
	}
}

Add-Process -Name "AuctionSvr" -Path "AuctionSvr.exe" -Arguments @()
Add-Process -Name "ChatSvr" -Path "ChatSvr.exe" -Arguments @()
Add-Process -Name "LoginSvr" -Path "LoginSvr.exe" -Arguments @()
Add-Process -Name "MasterSvr" -Path "MasterSvr.exe" -Arguments @()
Add-Process -Name "PartySvr" -Path "PartySvr.exe" -Arguments @()
Add-Process -Name "WorldSvr" -Path "WorldSvr.exe" -Arguments @("WorldSvr.ini")
#Add-Process -Name "WorldSvr02" -Path "WorldSvr02.exe" -Arguments @("WorldSvr_02.ini")
#Add-Process -Name "WorldSvr03" -Path "WorldSvr03.exe" -Arguments @("WorldSvr_03.ini")

Start-Monitoring

Cleanup