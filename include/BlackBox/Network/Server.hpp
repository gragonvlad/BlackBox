#pragma once
#include <BlackBox/INetwork.hpp>

class CServer : public IServer
{
	CServer();
	~CServer();
	// ������������ ����� IServer
	virtual void Update(unsigned int nTime) override;
	virtual void Release() override;
	virtual void SetVariable(CryNetworkVarible eVarName, unsigned int nValue) override;
	virtual void GetBandwidth(float& fIncomingKbPerSec, float& fOutgoinKbPerSec, uint32_t& nIncomingPackets, uint32_t& nOutgoingPackets) override;
	virtual const char* GetHostName() override;
	virtual void RegisterPacketSink(const unsigned char inPacketID, INetworkPacketSink* inpSink) override;
	virtual void SetSecuritySink(IServerSecuritySink* pSecuritySink) override;
	virtual bool IsIPBanned(const unsigned int dwIP) override;
	virtual void BanIP(const unsigned int dwIP) override;
	virtual void UnbanIP(const unsigned int dwIP) override;
	virtual IServerSlot* GetServerSlotbyID(const unsigned char ucId) const override;
	virtual uint8_t GetMaxClientID() const override;
	virtual EMPServerType GetServerType() const override;
};