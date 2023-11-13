// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/CouponUIWidget.h"
#include "Components/EditableTextBox.h"

#include "Network/Network.h"
#include "Global.h"

void UCouponUIWidget::TryUsingCoupon()
{
	const char* coupon = TCHAR_TO_ANSI(*CouponTextbox->GetText().ToString());
	send_coupon_packet(Network::GetNetwork()->l_socket, coupon);
}
