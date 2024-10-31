#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#define FB_BOT_API_VERSION "7.4"

#define FB_MAKE_HASH(x) constexpr size_t x = SH(#x);
#define FB_MAKE_EXTERN(x) extern const __FlashStringHelper* x;

namespace tg_api {
FB_MAKE_EXTERN(last_error_date)
FB_MAKE_EXTERN(year)
FB_MAKE_EXTERN(big_file_unique_id)
FB_MAKE_EXTERN(watcher)
FB_MAKE_EXTERN(external_reply)
FB_MAKE_EXTERN(resize_keyboard)
FB_MAKE_EXTERN(live_period)
FB_MAKE_EXTERN(forward_text)
FB_MAKE_EXTERN(request_photo)
FB_MAKE_EXTERN(allow_user_chats)
FB_MAKE_EXTERN(was_refunded)
FB_MAKE_EXTERN(can_connect_to_business)
FB_MAKE_EXTERN(winners)
FB_MAKE_EXTERN(input_message_content)
FB_MAKE_EXTERN(can_delete_messages)
FB_MAKE_EXTERN(message_text)
FB_MAKE_EXTERN(winner_count)
FB_MAKE_EXTERN(from)
FB_MAKE_EXTERN(heading)
FB_MAKE_EXTERN(last_error_message)
FB_MAKE_EXTERN(video_duration)
FB_MAKE_EXTERN(emoji_list)
FB_MAKE_EXTERN(command)
FB_MAKE_EXTERN(old_reaction)
FB_MAKE_EXTERN(url)
FB_MAKE_EXTERN(inline_message_id)
FB_MAKE_EXTERN(selfie)
FB_MAKE_EXTERN(limit)
FB_MAKE_EXTERN(show_caption_above_media)
FB_MAKE_EXTERN(max_connections)
FB_MAKE_EXTERN(google_place_id)
FB_MAKE_EXTERN(is_topic_message)
FB_MAKE_EXTERN(text_parse_mode)
FB_MAKE_EXTERN(dark_theme_dimming)
FB_MAKE_EXTERN(is_from_offline)
FB_MAKE_EXTERN(video_url)
FB_MAKE_EXTERN(can_send_videos)
FB_MAKE_EXTERN(has_hidden_members)
FB_MAKE_EXTERN(selective)
FB_MAKE_EXTERN(disable_notification)
FB_MAKE_EXTERN(add_date)
FB_MAKE_EXTERN(shipping_address)
FB_MAKE_EXTERN(is_moving)
FB_MAKE_EXTERN(migrate_from_chat_id)
FB_MAKE_EXTERN(only_if_banned)
FB_MAKE_EXTERN(left_chat_member)
FB_MAKE_EXTERN(drop_pending_updates)
FB_MAKE_EXTERN(need_email)
FB_MAKE_EXTERN(color)
FB_MAKE_EXTERN(can_send_video_notes)
FB_MAKE_EXTERN(giveaway_winners)
FB_MAKE_EXTERN(chat_background_set)
FB_MAKE_EXTERN(pre_checkout_query)
FB_MAKE_EXTERN(successful_payment)
FB_MAKE_EXTERN(callback_query)
FB_MAKE_EXTERN(correct_option_id)
FB_MAKE_EXTERN(max_tip_amount)
FB_MAKE_EXTERN(sender_boost_count)
FB_MAKE_EXTERN(from_attachment_menu)
FB_MAKE_EXTERN(message_auto_delete_time)
FB_MAKE_EXTERN(passport_data)
FB_MAKE_EXTERN(request_contact)
FB_MAKE_EXTERN(has_aggressive_anti_spam_enabled)
FB_MAKE_EXTERN(write_access_allowed)
FB_MAKE_EXTERN(giveaway)
FB_MAKE_EXTERN(video_chat_ended)
FB_MAKE_EXTERN(poll_id)
FB_MAKE_EXTERN(supports_streaming)
FB_MAKE_EXTERN(chat_type)
FB_MAKE_EXTERN(disable_content_type_detection)
FB_MAKE_EXTERN(additional_chat_count)
FB_MAKE_EXTERN(message_effect_id)
FB_MAKE_EXTERN(chat_has_username)
FB_MAKE_EXTERN(chosen_inline_result)
FB_MAKE_EXTERN(provider_payment_charge_id)
FB_MAKE_EXTERN(join_to_send_messages)
FB_MAKE_EXTERN(reply_markup)
FB_MAKE_EXTERN(longitude)
FB_MAKE_EXTERN(is_closed)
FB_MAKE_EXTERN(unclaimed_prize_count)
FB_MAKE_EXTERN(latitude)
FB_MAKE_EXTERN(show_above_text)
FB_MAKE_EXTERN(only_new_members)
FB_MAKE_EXTERN(use_independent_chat_permissions)
FB_MAKE_EXTERN(title)
FB_MAKE_EXTERN(start_parameter)
FB_MAKE_EXTERN(can_send_messages)
FB_MAKE_EXTERN(text_entities)
FB_MAKE_EXTERN(request_users)
FB_MAKE_EXTERN(files)
FB_MAKE_EXTERN(is_premium)
FB_MAKE_EXTERN(need_phone_number)
FB_MAKE_EXTERN(can_set_sticker_set)
FB_MAKE_EXTERN(language_code)
FB_MAKE_EXTERN(audio_file_id)
FB_MAKE_EXTERN(photo_width)
FB_MAKE_EXTERN(switch_inline_query)
FB_MAKE_EXTERN(switch_inline_query_chosen_chat)
FB_MAKE_EXTERN(voice_duration)
FB_MAKE_EXTERN(send_phone_number_to_provider)
FB_MAKE_EXTERN(edited_channel_post)
FB_MAKE_EXTERN(callback_query_id)
FB_MAKE_EXTERN(set_name)
FB_MAKE_EXTERN(file_path)
FB_MAKE_EXTERN(pending_join_request_count)
FB_MAKE_EXTERN(can_edit_stories)
FB_MAKE_EXTERN(link_preview_options)
FB_MAKE_EXTERN(mpeg4_url)
FB_MAKE_EXTERN(forum_topic_created)
FB_MAKE_EXTERN(bot_administrator_rights)
FB_MAKE_EXTERN(expire_date)
FB_MAKE_EXTERN(results)
FB_MAKE_EXTERN(gif_file_id)
FB_MAKE_EXTERN(personal_chat)
FB_MAKE_EXTERN(allows_multiple_answers)
FB_MAKE_EXTERN(forum_topic_edited)
FB_MAKE_EXTERN(bottom_color)
FB_MAKE_EXTERN(rotation_angle)
FB_MAKE_EXTERN(allow_group_chats)
FB_MAKE_EXTERN(document_file_id)
FB_MAKE_EXTERN(media)
FB_MAKE_EXTERN(business_connection_id)
FB_MAKE_EXTERN(field_name)
FB_MAKE_EXTERN(stickers)
FB_MAKE_EXTERN(is_flexible)
FB_MAKE_EXTERN(keywords)
FB_MAKE_EXTERN(mpeg4_duration)
FB_MAKE_EXTERN(remove_keyboard)
FB_MAKE_EXTERN(game)
FB_MAKE_EXTERN(chat_join_request)
FB_MAKE_EXTERN(scale)
FB_MAKE_EXTERN(message_auto_delete_timer_changed)
FB_MAKE_EXTERN(chat_is_forum)
FB_MAKE_EXTERN(callback_data)
FB_MAKE_EXTERN(emoji)
FB_MAKE_EXTERN(has_spoiler)
FB_MAKE_EXTERN(allow_bot_chats)
FB_MAKE_EXTERN(boost_added)
FB_MAKE_EXTERN(caption_entities)
FB_MAKE_EXTERN(user)
FB_MAKE_EXTERN(delete_chat_photo)
FB_MAKE_EXTERN(video_chat_participants_invited)
FB_MAKE_EXTERN(reactions)
FB_MAKE_EXTERN(giveaway_created)
FB_MAKE_EXTERN(switch_inline_query_current_chat)
FB_MAKE_EXTERN(scope)
FB_MAKE_EXTERN(reverse_side)
FB_MAKE_EXTERN(street_line2)
FB_MAKE_EXTERN(message_id)
FB_MAKE_EXTERN(mpeg4_width)
FB_MAKE_EXTERN(explanation)
FB_MAKE_EXTERN(pinned_message)
FB_MAKE_EXTERN(slow_mode_delay)
FB_MAKE_EXTERN(can_send_documents)
FB_MAKE_EXTERN(reaction)
FB_MAKE_EXTERN(new_chat_member)
FB_MAKE_EXTERN(id)
FB_MAKE_EXTERN(supports_inline_queries)
FB_MAKE_EXTERN(fill)
FB_MAKE_EXTERN(is_persistent)
FB_MAKE_EXTERN(start_date)
FB_MAKE_EXTERN(photo_size)
FB_MAKE_EXTERN(allow_channel_chats)
FB_MAKE_EXTERN(active_usernames)
FB_MAKE_EXTERN(country_codes)
FB_MAKE_EXTERN(custom_emoji_id)
FB_MAKE_EXTERN(business_opening_hours)
FB_MAKE_EXTERN(creator)
FB_MAKE_EXTERN(thumbnail)
FB_MAKE_EXTERN(chat_is_created)
FB_MAKE_EXTERN(join_by_request)
FB_MAKE_EXTERN(x_shift)
FB_MAKE_EXTERN(sticker_format)
FB_MAKE_EXTERN(options)
FB_MAKE_EXTERN(width)
FB_MAKE_EXTERN(remove_date)
FB_MAKE_EXTERN(video_width)
FB_MAKE_EXTERN(can_promote_members)
FB_MAKE_EXTERN(shipping_options)
FB_MAKE_EXTERN(can_send_other_messages)
FB_MAKE_EXTERN(y_shift)
FB_MAKE_EXTERN(parse_mode)
FB_MAKE_EXTERN(user_id)
FB_MAKE_EXTERN(theme_name)
FB_MAKE_EXTERN(force_reply)
FB_MAKE_EXTERN(is_animated)
FB_MAKE_EXTERN(gif_height)
FB_MAKE_EXTERN(can_edit_messages)
FB_MAKE_EXTERN(keyboard)
FB_MAKE_EXTERN(old_chat_member)
FB_MAKE_EXTERN(voice)
FB_MAKE_EXTERN(request_poll)
FB_MAKE_EXTERN(prize_description)
FB_MAKE_EXTERN(intensity)
FB_MAKE_EXTERN(added_to_attachment_menu)
FB_MAKE_EXTERN(has_custom_certificate)
FB_MAKE_EXTERN(value)
FB_MAKE_EXTERN(poll)
FB_MAKE_EXTERN(close_date)
FB_MAKE_EXTERN(photos)
FB_MAKE_EXTERN(request_title)
FB_MAKE_EXTERN(bot_is_member)
FB_MAKE_EXTERN(can_invite_users)
FB_MAKE_EXTERN(request_location)
FB_MAKE_EXTERN(file_unique_id)
FB_MAKE_EXTERN(menu_button)
FB_MAKE_EXTERN(error_message)
FB_MAKE_EXTERN(sticker)
FB_MAKE_EXTERN(can_add_web_page_previews)
FB_MAKE_EXTERN(new_chat_title)
FB_MAKE_EXTERN(contact)
FB_MAKE_EXTERN(opening_minute)
FB_MAKE_EXTERN(connected_website)
FB_MAKE_EXTERN(new_chat_photo)
FB_MAKE_EXTERN(colors)
FB_MAKE_EXTERN(sender_chat_id)
FB_MAKE_EXTERN(secret)
FB_MAKE_EXTERN(file_hash)
FB_MAKE_EXTERN(currency)
FB_MAKE_EXTERN(inline_query)
FB_MAKE_EXTERN(author_signature)
FB_MAKE_EXTERN(proximity_alert_radius)
FB_MAKE_EXTERN(member_limit)
FB_MAKE_EXTERN(text)
FB_MAKE_EXTERN(animation)
FB_MAKE_EXTERN(from_request)
FB_MAKE_EXTERN(premium_subscription_month_count)
FB_MAKE_EXTERN(first_name)
FB_MAKE_EXTERN(order_info)
FB_MAKE_EXTERN(vcard)
FB_MAKE_EXTERN(via_bot)
FB_MAKE_EXTERN(bio)
FB_MAKE_EXTERN(allow_sending_without_reply)
FB_MAKE_EXTERN(chat)
FB_MAKE_EXTERN(is_disabled)
FB_MAKE_EXTERN(data)
FB_MAKE_EXTERN(permissions)
FB_MAKE_EXTERN(emoji_status_custom_emoji_id)
FB_MAKE_EXTERN(until_date)
FB_MAKE_EXTERN(edit_date)
FB_MAKE_EXTERN(invite_link)
FB_MAKE_EXTERN(channel_chat_created)
FB_MAKE_EXTERN(total_voter_count)
FB_MAKE_EXTERN(data_hash)
FB_MAKE_EXTERN(boost_count)
FB_MAKE_EXTERN(message)
FB_MAKE_EXTERN(show_alert)
FB_MAKE_EXTERN(question)
FB_MAKE_EXTERN(amount)
FB_MAKE_EXTERN(custom_title)
FB_MAKE_EXTERN(general_forum_topic_unhidden)
FB_MAKE_EXTERN(photo_file_id)
FB_MAKE_EXTERN(chat_id)
FB_MAKE_EXTERN(thumbnail_mime_type)
FB_MAKE_EXTERN(protect_content)
FB_MAKE_EXTERN(has_public_winners)
FB_MAKE_EXTERN(point)
FB_MAKE_EXTERN(gif_url)
FB_MAKE_EXTERN(username)
FB_MAKE_EXTERN(is_big)
FB_MAKE_EXTERN(question_parse_mode)
FB_MAKE_EXTERN(is_personal)
FB_MAKE_EXTERN(suggested_tip_amounts)
FB_MAKE_EXTERN(users_shared)
FB_MAKE_EXTERN(bot_username)
FB_MAKE_EXTERN(secret_token)
FB_MAKE_EXTERN(traveler)
FB_MAKE_EXTERN(ip_address)
FB_MAKE_EXTERN(telegram_payment_charge_id)
FB_MAKE_EXTERN(max_quantity)
FB_MAKE_EXTERN(revoke_messages)
FB_MAKE_EXTERN(offset)
FB_MAKE_EXTERN(photo)
FB_MAKE_EXTERN(web_app_query_id)
FB_MAKE_EXTERN(venue)
FB_MAKE_EXTERN(query)
FB_MAKE_EXTERN(audio)
FB_MAKE_EXTERN(month)
FB_MAKE_EXTERN(request_id)
FB_MAKE_EXTERN(edited_business_message)
FB_MAKE_EXTERN(result)
FB_MAKE_EXTERN(winners_selection_date)
FB_MAKE_EXTERN(chat_instance)
FB_MAKE_EXTERN(force)
FB_MAKE_EXTERN(last_name)
FB_MAKE_EXTERN(from_chat_id)
FB_MAKE_EXTERN(is_revoked)
FB_MAKE_EXTERN(disable_edit_message)
FB_MAKE_EXTERN(thumbnail_url)
FB_MAKE_EXTERN(document)
FB_MAKE_EXTERN(allowed_updates)
FB_MAKE_EXTERN(mask_position)
FB_MAKE_EXTERN(chat_member)
FB_MAKE_EXTERN(business_intro)
FB_MAKE_EXTERN(is_member)
FB_MAKE_EXTERN(channel_post)
FB_MAKE_EXTERN(hide_url)
FB_MAKE_EXTERN(credentials)
FB_MAKE_EXTERN(is_bot)
FB_MAKE_EXTERN(update_id)
FB_MAKE_EXTERN(via_chat_folder_invite_link)
FB_MAKE_EXTERN(request_chat)
FB_MAKE_EXTERN(max_reaction_count)
FB_MAKE_EXTERN(small_file_unique_id)
FB_MAKE_EXTERN(can_change_info)
FB_MAKE_EXTERN(distance)
FB_MAKE_EXTERN(can_read_all_group_messages)
FB_MAKE_EXTERN(boosts)
FB_MAKE_EXTERN(poll_answer)
FB_MAKE_EXTERN(general_forum_topic_hidden)
FB_MAKE_EXTERN(google_place_type)
FB_MAKE_EXTERN(entities)
FB_MAKE_EXTERN(custom_emoji_sticker_set_name)
FB_MAKE_EXTERN(birthdate)
FB_MAKE_EXTERN(mpeg4_file_id)
FB_MAKE_EXTERN(provider_data)
FB_MAKE_EXTERN(total_amount)
FB_MAKE_EXTERN(inline_keyboard)
FB_MAKE_EXTERN(format)
FB_MAKE_EXTERN(need_shipping_address)
FB_MAKE_EXTERN(thumbnail_width)
FB_MAKE_EXTERN(sender_chat)
FB_MAKE_EXTERN(request_name)
FB_MAKE_EXTERN(can_be_edited)
FB_MAKE_EXTERN(giveaway_completed)
FB_MAKE_EXTERN(quote_parse_mode)
FB_MAKE_EXTERN(foursquare_id)
FB_MAKE_EXTERN(boost)
FB_MAKE_EXTERN(element_hash)
FB_MAKE_EXTERN(thumbnail_height)
FB_MAKE_EXTERN(video_chat_started)
FB_MAKE_EXTERN(mime_type)
FB_MAKE_EXTERN(provider_token)
FB_MAKE_EXTERN(can_send_audios)
FB_MAKE_EXTERN(via_join_request)
FB_MAKE_EXTERN(message_reaction)
FB_MAKE_EXTERN(can_manage_topics)
FB_MAKE_EXTERN(can_join_groups)
FB_MAKE_EXTERN(can_send_polls)
FB_MAKE_EXTERN(edited_message)
FB_MAKE_EXTERN(can_manage_video_chats)
FB_MAKE_EXTERN(sender_user_name)
FB_MAKE_EXTERN(available_reactions)
FB_MAKE_EXTERN(voter_count)
FB_MAKE_EXTERN(forum_topic_closed)
FB_MAKE_EXTERN(source)
FB_MAKE_EXTERN(prefer_large_media)
FB_MAKE_EXTERN(timeout)
FB_MAKE_EXTERN(prefer_small_media)
FB_MAKE_EXTERN(migrate_to_chat_id)
FB_MAKE_EXTERN(creates_join_request)
FB_MAKE_EXTERN(needs_repainting)
FB_MAKE_EXTERN(web_app_data)
FB_MAKE_EXTERN(day)
FB_MAKE_EXTERN(video_note)
FB_MAKE_EXTERN(foursquare_type)
FB_MAKE_EXTERN(can_send_voice_notes)
FB_MAKE_EXTERN(can_send_photos)
FB_MAKE_EXTERN(can_restrict_members)
FB_MAKE_EXTERN(business_location)
FB_MAKE_EXTERN(option_ids)
FB_MAKE_EXTERN(sticker_type)
FB_MAKE_EXTERN(payload)
FB_MAKE_EXTERN(profile_background_custom_emoji_id)
FB_MAKE_EXTERN(shipping_query)
FB_MAKE_EXTERN(is_video)
FB_MAKE_EXTERN(country_code)
FB_MAKE_EXTERN(prices)
FB_MAKE_EXTERN(shipping_option_id)
FB_MAKE_EXTERN(next_offset)
FB_MAKE_EXTERN(short_description)
FB_MAKE_EXTERN(custom_emoji_ids)
FB_MAKE_EXTERN(has_private_forwards)
FB_MAKE_EXTERN(status)
FB_MAKE_EXTERN(request_username)
FB_MAKE_EXTERN(for_channels)
FB_MAKE_EXTERN(video_file_id)
FB_MAKE_EXTERN(input_field_placeholder)
FB_MAKE_EXTERN(post_code)
FB_MAKE_EXTERN(retry_after)
FB_MAKE_EXTERN(can_post_stories)
FB_MAKE_EXTERN(web_app)
FB_MAKE_EXTERN(name)
FB_MAKE_EXTERN(can_reply)
FB_MAKE_EXTERN(video_chat_scheduled)
FB_MAKE_EXTERN(is_inverted)
FB_MAKE_EXTERN(chats)
FB_MAKE_EXTERN(last_synchronization_error_date)
FB_MAKE_EXTERN(profile_accent_color_id)
FB_MAKE_EXTERN(sender_user)
FB_MAKE_EXTERN(request_write_access)
FB_MAKE_EXTERN(audio_duration)
FB_MAKE_EXTERN(ok)
FB_MAKE_EXTERN(total_count)
FB_MAKE_EXTERN(one_time_keyboard)
FB_MAKE_EXTERN(invoice_payload)
FB_MAKE_EXTERN(removed_chat_boost)
FB_MAKE_EXTERN(type)
FB_MAKE_EXTERN(shipping_query_id)
FB_MAKE_EXTERN(inline_query_id)
FB_MAKE_EXTERN(audio_url)
FB_MAKE_EXTERN(top_color)
FB_MAKE_EXTERN(height)
FB_MAKE_EXTERN(phone_number)
FB_MAKE_EXTERN(forum_topic_reopened)
FB_MAKE_EXTERN(my_chat_member)
FB_MAKE_EXTERN(emoji_status_expiration_date)
FB_MAKE_EXTERN(front_side)
FB_MAKE_EXTERN(message_thread_id)
FB_MAKE_EXTERN(city)
FB_MAKE_EXTERN(certificate)
FB_MAKE_EXTERN(message_ids)
FB_MAKE_EXTERN(closing_minute)
FB_MAKE_EXTERN(file_id)
FB_MAKE_EXTERN(quote_entities)
FB_MAKE_EXTERN(chat_is_channel)
FB_MAKE_EXTERN(voice_url)
FB_MAKE_EXTERN(icon_color)
FB_MAKE_EXTERN(login_url)
FB_MAKE_EXTERN(button)
FB_MAKE_EXTERN(description)
FB_MAKE_EXTERN(remove_caption)
FB_MAKE_EXTERN(mpeg4_height)
FB_MAKE_EXTERN(state)
FB_MAKE_EXTERN(language)
FB_MAKE_EXTERN(giveaway_message)
FB_MAKE_EXTERN(can_manage_chat)
FB_MAKE_EXTERN(pre_checkout_query_id)
FB_MAKE_EXTERN(file_date)
FB_MAKE_EXTERN(file_hashes)
FB_MAKE_EXTERN(boost_id)
FB_MAKE_EXTERN(unrestrict_boost_count)
FB_MAKE_EXTERN(accent_color_id)
FB_MAKE_EXTERN(chat_shared)
FB_MAKE_EXTERN(user_chat_id)
FB_MAKE_EXTERN(opening_hours)
FB_MAKE_EXTERN(gif_width)
FB_MAKE_EXTERN(action)
FB_MAKE_EXTERN(horizontal_accuracy)
FB_MAKE_EXTERN(icon_custom_emoji_id)
FB_MAKE_EXTERN(is_unclaimed)
FB_MAKE_EXTERN(is_anonymous)
FB_MAKE_EXTERN(user_administrator_rights)
FB_MAKE_EXTERN(is_manual)
FB_MAKE_EXTERN(has_protected_content)
FB_MAKE_EXTERN(email)
FB_MAKE_EXTERN(new_chat_members)
FB_MAKE_EXTERN(expiration_date)
FB_MAKE_EXTERN(media_group_id)
FB_MAKE_EXTERN(user_is_bot)
FB_MAKE_EXTERN(gif_duration)
FB_MAKE_EXTERN(video_height)
FB_MAKE_EXTERN(has_restricted_voice_and_video_messages)
FB_MAKE_EXTERN(message_reaction_count)
FB_MAKE_EXTERN(performer)
FB_MAKE_EXTERN(button_text)
FB_MAKE_EXTERN(actor_chat)
FB_MAKE_EXTERN(business_connection)
FB_MAKE_EXTERN(reply_parameters)
FB_MAKE_EXTERN(length)
FB_MAKE_EXTERN(user_is_premium)
FB_MAKE_EXTERN(old_sticker)
FB_MAKE_EXTERN(voice_file_id)
FB_MAKE_EXTERN(deleted_business_messages)
FB_MAKE_EXTERN(origin)
FB_MAKE_EXTERN(pending_update_count)
FB_MAKE_EXTERN(sender_business_bot)
FB_MAKE_EXTERN(premium_animation)
FB_MAKE_EXTERN(forward_origin)
FB_MAKE_EXTERN(can_delete_stories)
FB_MAKE_EXTERN(open_period)
FB_MAKE_EXTERN(duration)
FB_MAKE_EXTERN(has_visible_history)
FB_MAKE_EXTERN(quote)
FB_MAKE_EXTERN(label)
FB_MAKE_EXTERN(photo_height)
FB_MAKE_EXTERN(hash)
FB_MAKE_EXTERN(effect_id)
FB_MAKE_EXTERN(chat_boost)
FB_MAKE_EXTERN(address)
FB_MAKE_EXTERN(reply_to_message)
FB_MAKE_EXTERN(score)
FB_MAKE_EXTERN(proximity_alert_triggered)
FB_MAKE_EXTERN(business_message)
FB_MAKE_EXTERN(big_file_id)
FB_MAKE_EXTERN(caption)
FB_MAKE_EXTERN(group_chat_created)
FB_MAKE_EXTERN(street_line1)
FB_MAKE_EXTERN(need_name)
FB_MAKE_EXTERN(is_forum)
FB_MAKE_EXTERN(story)
FB_MAKE_EXTERN(explanation_entities)
FB_MAKE_EXTERN(voter_chat)
FB_MAKE_EXTERN(file_name)
FB_MAKE_EXTERN(send_email_to_provider)
FB_MAKE_EXTERN(document_url)
FB_MAKE_EXTERN(is_automatic_forward)
FB_MAKE_EXTERN(game_short_name)
FB_MAKE_EXTERN(photo_url)
FB_MAKE_EXTERN(sticker_file_id)
FB_MAKE_EXTERN(file_size)
FB_MAKE_EXTERN(callback_game)
FB_MAKE_EXTERN(result_id)
FB_MAKE_EXTERN(is_primary)
FB_MAKE_EXTERN(video)
FB_MAKE_EXTERN(supergroup_chat_created)
FB_MAKE_EXTERN(rights)
FB_MAKE_EXTERN(question_entities)
FB_MAKE_EXTERN(new_reaction)
FB_MAKE_EXTERN(giveaway_message_id)
FB_MAKE_EXTERN(is_enabled)
FB_MAKE_EXTERN(translation)
FB_MAKE_EXTERN(can_post_messages)
FB_MAKE_EXTERN(explanation_parse_mode)
FB_MAKE_EXTERN(web_app_name)
FB_MAKE_EXTERN(reply_to_story)
FB_MAKE_EXTERN(has_media_spoiler)
FB_MAKE_EXTERN(can_pin_messages)
FB_MAKE_EXTERN(background_custom_emoji_id)
FB_MAKE_EXTERN(cache_time)
FB_MAKE_EXTERN(date)
FB_MAKE_EXTERN(quote_position)
FB_MAKE_EXTERN(position)
FB_MAKE_EXTERN(dice)
FB_MAKE_EXTERN(linked_chat_id)
FB_MAKE_EXTERN(invoice)
FB_MAKE_EXTERN(is_blurred)
FB_MAKE_EXTERN(sticker_set_name)
FB_MAKE_EXTERN(location)
FB_MAKE_EXTERN(errors)
FB_MAKE_EXTERN(small_file_id)
FB_MAKE_EXTERN(time_zone_name)
FB_MAKE_EXTERN(pay)
FB_MAKE_EXTERN(commands)
FB_MAKE_EXTERN(users)
}  // namespace tg_api

namespace tg_cmd {
FB_MAKE_EXTERN(unpinAllChatMessages)
FB_MAKE_EXTERN(getChatAdministrators)
FB_MAKE_EXTERN(setMyShortDescription)
FB_MAKE_EXTERN(setMyDefaultAdministratorRights)
FB_MAKE_EXTERN(sendMessage)
FB_MAKE_EXTERN(getMyDefaultAdministratorRights)
FB_MAKE_EXTERN(sendPoll)
FB_MAKE_EXTERN(sendPhoto)
FB_MAKE_EXTERN(restrictChatMember)
FB_MAKE_EXTERN(getForumTopicIconStickers)
FB_MAKE_EXTERN(forwardMessages)
FB_MAKE_EXTERN(sendMediaGroup)
FB_MAKE_EXTERN(answerWebAppQuery)
FB_MAKE_EXTERN(sendVoice)
FB_MAKE_EXTERN(revokeChatInviteLink)
FB_MAKE_EXTERN(setStickerKeywords)
FB_MAKE_EXTERN(setChatPhoto)
FB_MAKE_EXTERN(deleteMyCommands)
FB_MAKE_EXTERN(getUpdates)
FB_MAKE_EXTERN(forwardMessage)
FB_MAKE_EXTERN(getChatMember)
FB_MAKE_EXTERN(getMyCommands)
FB_MAKE_EXTERN(editChatInviteLink)
FB_MAKE_EXTERN(closeGeneralForumTopic)
FB_MAKE_EXTERN(setStickerMaskPosition)
FB_MAKE_EXTERN(editMessageLiveLocation)
FB_MAKE_EXTERN(editGeneralForumTopic)
FB_MAKE_EXTERN(sendDocument)
FB_MAKE_EXTERN(sendVideo)
FB_MAKE_EXTERN(copyMessages)
FB_MAKE_EXTERN(getWebhookInfo)
FB_MAKE_EXTERN(setChatStickerSet)
FB_MAKE_EXTERN(uploadStickerFile)
FB_MAKE_EXTERN(setChatAdministratorCustomTitle)
FB_MAKE_EXTERN(answerCallbackQuery)
FB_MAKE_EXTERN(getMyDescription)
FB_MAKE_EXTERN(setStickerSetTitle)
FB_MAKE_EXTERN(setMessageReaction)
FB_MAKE_EXTERN(setChatTitle)
FB_MAKE_EXTERN(createForumTopic)
FB_MAKE_EXTERN(setGameScore)
FB_MAKE_EXTERN(sendAnimation)
FB_MAKE_EXTERN(unbanChatSenderChat)
FB_MAKE_EXTERN(addStickerToSet)
FB_MAKE_EXTERN(deleteMessage)
FB_MAKE_EXTERN(sendContact)
FB_MAKE_EXTERN(replaceStickerInSet)
FB_MAKE_EXTERN(setChatMenuButton)
FB_MAKE_EXTERN(setMyName)
FB_MAKE_EXTERN(setPassportDataErrors)
FB_MAKE_EXTERN(setChatDescription)
FB_MAKE_EXTERN(closeForumTopic)
FB_MAKE_EXTERN(getMe)
FB_MAKE_EXTERN(banChatMember)
FB_MAKE_EXTERN(setStickerPositionInSet)
FB_MAKE_EXTERN(close)
FB_MAKE_EXTERN(unpinChatMessage)
FB_MAKE_EXTERN(getStickerSet)
FB_MAKE_EXTERN(sendDice)
FB_MAKE_EXTERN(createNewStickerSet)
FB_MAKE_EXTERN(setChatPermissions)
FB_MAKE_EXTERN(answerPreCheckoutQuery)
FB_MAKE_EXTERN(sendGame)
FB_MAKE_EXTERN(editMessageMedia)
FB_MAKE_EXTERN(deleteChatStickerSet)
FB_MAKE_EXTERN(sendVideoNote)
FB_MAKE_EXTERN(logOut)
FB_MAKE_EXTERN(sendSticker)
FB_MAKE_EXTERN(editForumTopic)
FB_MAKE_EXTERN(reopenGeneralForumTopic)
FB_MAKE_EXTERN(refundStarPayment)
FB_MAKE_EXTERN(copyMessage)
FB_MAKE_EXTERN(sendLocation)
FB_MAKE_EXTERN(deleteMessages)
FB_MAKE_EXTERN(getChat)
FB_MAKE_EXTERN(leaveChat)
FB_MAKE_EXTERN(getUserChatBoosts)
FB_MAKE_EXTERN(editMessageCaption)
FB_MAKE_EXTERN(approveChatJoinRequest)
FB_MAKE_EXTERN(answerShippingQuery)
FB_MAKE_EXTERN(sendChatAction)
FB_MAKE_EXTERN(createInvoiceLink)
FB_MAKE_EXTERN(setWebhook)
FB_MAKE_EXTERN(reopenForumTopic)
FB_MAKE_EXTERN(unpinAllForumTopicMessages)
FB_MAKE_EXTERN(deleteStickerSet)
FB_MAKE_EXTERN(getMyShortDescription)
FB_MAKE_EXTERN(deleteWebhook)
FB_MAKE_EXTERN(editMessageReplyMarkup)
FB_MAKE_EXTERN(answerInlineQuery)
FB_MAKE_EXTERN(setStickerEmojiList)
FB_MAKE_EXTERN(getGameHighScores)
FB_MAKE_EXTERN(getUserProfilePhotos)
FB_MAKE_EXTERN(createChatInviteLink)
FB_MAKE_EXTERN(deleteForumTopic)
FB_MAKE_EXTERN(deleteChatPhoto)
FB_MAKE_EXTERN(getChatMemberCount)
FB_MAKE_EXTERN(setMyCommands)
FB_MAKE_EXTERN(getFile)
FB_MAKE_EXTERN(getChatMenuButton)
FB_MAKE_EXTERN(getBusinessConnection)
FB_MAKE_EXTERN(declineChatJoinRequest)
FB_MAKE_EXTERN(getMyName)
FB_MAKE_EXTERN(sendVenue)
FB_MAKE_EXTERN(banChatSenderChat)
FB_MAKE_EXTERN(deleteStickerFromSet)
FB_MAKE_EXTERN(pinChatMessage)
FB_MAKE_EXTERN(promoteChatMember)
FB_MAKE_EXTERN(editMessageText)
FB_MAKE_EXTERN(stopPoll)
FB_MAKE_EXTERN(getCustomEmojiStickers)
FB_MAKE_EXTERN(unbanChatMember)
FB_MAKE_EXTERN(hideGeneralForumTopic)
FB_MAKE_EXTERN(unhideGeneralForumTopic)
FB_MAKE_EXTERN(stopMessageLiveLocation)
FB_MAKE_EXTERN(sendAudio)
FB_MAKE_EXTERN(setStickerSetThumbnail)
FB_MAKE_EXTERN(setCustomEmojiStickerSetThumbnail)
FB_MAKE_EXTERN(exportChatInviteLink)
FB_MAKE_EXTERN(setMyDescription)
FB_MAKE_EXTERN(unpinAllGeneralForumTopicMessages)
FB_MAKE_EXTERN(sendInvoice)
}  // namespace tg_cmd

namespace tg_apih {
FB_MAKE_HASH(last_error_date)
FB_MAKE_HASH(year)
FB_MAKE_HASH(big_file_unique_id)
FB_MAKE_HASH(watcher)
FB_MAKE_HASH(external_reply)
FB_MAKE_HASH(resize_keyboard)
FB_MAKE_HASH(live_period)
FB_MAKE_HASH(forward_text)
FB_MAKE_HASH(request_photo)
FB_MAKE_HASH(allow_user_chats)
FB_MAKE_HASH(was_refunded)
FB_MAKE_HASH(can_connect_to_business)
FB_MAKE_HASH(winners)
FB_MAKE_HASH(input_message_content)
FB_MAKE_HASH(can_delete_messages)
FB_MAKE_HASH(message_text)
FB_MAKE_HASH(winner_count)
FB_MAKE_HASH(from)
FB_MAKE_HASH(heading)
FB_MAKE_HASH(last_error_message)
FB_MAKE_HASH(video_duration)
FB_MAKE_HASH(emoji_list)
FB_MAKE_HASH(command)
FB_MAKE_HASH(old_reaction)
FB_MAKE_HASH(url)
FB_MAKE_HASH(inline_message_id)
FB_MAKE_HASH(selfie)
FB_MAKE_HASH(limit)
FB_MAKE_HASH(show_caption_above_media)
FB_MAKE_HASH(max_connections)
FB_MAKE_HASH(google_place_id)
FB_MAKE_HASH(is_topic_message)
FB_MAKE_HASH(text_parse_mode)
FB_MAKE_HASH(dark_theme_dimming)
FB_MAKE_HASH(is_from_offline)
FB_MAKE_HASH(video_url)
FB_MAKE_HASH(can_send_videos)
FB_MAKE_HASH(has_hidden_members)
FB_MAKE_HASH(selective)
FB_MAKE_HASH(disable_notification)
FB_MAKE_HASH(add_date)
FB_MAKE_HASH(shipping_address)
FB_MAKE_HASH(is_moving)
FB_MAKE_HASH(migrate_from_chat_id)
FB_MAKE_HASH(only_if_banned)
FB_MAKE_HASH(left_chat_member)
FB_MAKE_HASH(drop_pending_updates)
FB_MAKE_HASH(need_email)
FB_MAKE_HASH(color)
FB_MAKE_HASH(can_send_video_notes)
FB_MAKE_HASH(giveaway_winners)
FB_MAKE_HASH(chat_background_set)
FB_MAKE_HASH(pre_checkout_query)
FB_MAKE_HASH(successful_payment)
FB_MAKE_HASH(callback_query)
FB_MAKE_HASH(correct_option_id)
FB_MAKE_HASH(max_tip_amount)
FB_MAKE_HASH(sender_boost_count)
FB_MAKE_HASH(from_attachment_menu)
FB_MAKE_HASH(message_auto_delete_time)
FB_MAKE_HASH(passport_data)
FB_MAKE_HASH(request_contact)
FB_MAKE_HASH(has_aggressive_anti_spam_enabled)
FB_MAKE_HASH(write_access_allowed)
FB_MAKE_HASH(giveaway)
FB_MAKE_HASH(video_chat_ended)
FB_MAKE_HASH(poll_id)
FB_MAKE_HASH(supports_streaming)
FB_MAKE_HASH(chat_type)
FB_MAKE_HASH(disable_content_type_detection)
FB_MAKE_HASH(additional_chat_count)
FB_MAKE_HASH(message_effect_id)
FB_MAKE_HASH(chat_has_username)
FB_MAKE_HASH(chosen_inline_result)
FB_MAKE_HASH(provider_payment_charge_id)
FB_MAKE_HASH(join_to_send_messages)
FB_MAKE_HASH(reply_markup)
FB_MAKE_HASH(longitude)
FB_MAKE_HASH(is_closed)
FB_MAKE_HASH(unclaimed_prize_count)
FB_MAKE_HASH(latitude)
FB_MAKE_HASH(show_above_text)
FB_MAKE_HASH(only_new_members)
FB_MAKE_HASH(use_independent_chat_permissions)
FB_MAKE_HASH(title)
FB_MAKE_HASH(start_parameter)
FB_MAKE_HASH(can_send_messages)
FB_MAKE_HASH(text_entities)
FB_MAKE_HASH(request_users)
FB_MAKE_HASH(files)
FB_MAKE_HASH(is_premium)
FB_MAKE_HASH(need_phone_number)
FB_MAKE_HASH(can_set_sticker_set)
FB_MAKE_HASH(language_code)
FB_MAKE_HASH(audio_file_id)
FB_MAKE_HASH(photo_width)
FB_MAKE_HASH(switch_inline_query)
FB_MAKE_HASH(switch_inline_query_chosen_chat)
FB_MAKE_HASH(voice_duration)
FB_MAKE_HASH(send_phone_number_to_provider)
FB_MAKE_HASH(edited_channel_post)
FB_MAKE_HASH(callback_query_id)
FB_MAKE_HASH(set_name)
FB_MAKE_HASH(file_path)
FB_MAKE_HASH(pending_join_request_count)
FB_MAKE_HASH(can_edit_stories)
FB_MAKE_HASH(link_preview_options)
FB_MAKE_HASH(mpeg4_url)
FB_MAKE_HASH(forum_topic_created)
FB_MAKE_HASH(bot_administrator_rights)
FB_MAKE_HASH(expire_date)
FB_MAKE_HASH(results)
FB_MAKE_HASH(gif_file_id)
FB_MAKE_HASH(personal_chat)
FB_MAKE_HASH(allows_multiple_answers)
FB_MAKE_HASH(forum_topic_edited)
FB_MAKE_HASH(bottom_color)
FB_MAKE_HASH(rotation_angle)
FB_MAKE_HASH(allow_group_chats)
FB_MAKE_HASH(document_file_id)
FB_MAKE_HASH(media)
FB_MAKE_HASH(business_connection_id)
FB_MAKE_HASH(field_name)
FB_MAKE_HASH(stickers)
FB_MAKE_HASH(is_flexible)
FB_MAKE_HASH(keywords)
FB_MAKE_HASH(mpeg4_duration)
FB_MAKE_HASH(remove_keyboard)
FB_MAKE_HASH(game)
FB_MAKE_HASH(chat_join_request)
FB_MAKE_HASH(scale)
FB_MAKE_HASH(message_auto_delete_timer_changed)
FB_MAKE_HASH(chat_is_forum)
FB_MAKE_HASH(callback_data)
FB_MAKE_HASH(emoji)
FB_MAKE_HASH(has_spoiler)
FB_MAKE_HASH(allow_bot_chats)
FB_MAKE_HASH(boost_added)
FB_MAKE_HASH(caption_entities)
FB_MAKE_HASH(user)
FB_MAKE_HASH(delete_chat_photo)
FB_MAKE_HASH(video_chat_participants_invited)
FB_MAKE_HASH(reactions)
FB_MAKE_HASH(giveaway_created)
FB_MAKE_HASH(switch_inline_query_current_chat)
FB_MAKE_HASH(scope)
FB_MAKE_HASH(reverse_side)
FB_MAKE_HASH(street_line2)
FB_MAKE_HASH(message_id)
FB_MAKE_HASH(mpeg4_width)
FB_MAKE_HASH(explanation)
FB_MAKE_HASH(pinned_message)
FB_MAKE_HASH(slow_mode_delay)
FB_MAKE_HASH(can_send_documents)
FB_MAKE_HASH(reaction)
FB_MAKE_HASH(new_chat_member)
FB_MAKE_HASH(id)
FB_MAKE_HASH(supports_inline_queries)
FB_MAKE_HASH(fill)
FB_MAKE_HASH(is_persistent)
FB_MAKE_HASH(start_date)
FB_MAKE_HASH(photo_size)
FB_MAKE_HASH(allow_channel_chats)
FB_MAKE_HASH(active_usernames)
FB_MAKE_HASH(country_codes)
FB_MAKE_HASH(custom_emoji_id)
FB_MAKE_HASH(business_opening_hours)
FB_MAKE_HASH(creator)
FB_MAKE_HASH(thumbnail)
FB_MAKE_HASH(chat_is_created)
FB_MAKE_HASH(join_by_request)
FB_MAKE_HASH(x_shift)
FB_MAKE_HASH(sticker_format)
FB_MAKE_HASH(options)
FB_MAKE_HASH(width)
FB_MAKE_HASH(remove_date)
FB_MAKE_HASH(video_width)
FB_MAKE_HASH(can_promote_members)
FB_MAKE_HASH(shipping_options)
FB_MAKE_HASH(can_send_other_messages)
FB_MAKE_HASH(y_shift)
FB_MAKE_HASH(parse_mode)
FB_MAKE_HASH(user_id)
FB_MAKE_HASH(theme_name)
FB_MAKE_HASH(force_reply)
FB_MAKE_HASH(is_animated)
FB_MAKE_HASH(gif_height)
FB_MAKE_HASH(can_edit_messages)
FB_MAKE_HASH(keyboard)
FB_MAKE_HASH(old_chat_member)
FB_MAKE_HASH(voice)
FB_MAKE_HASH(request_poll)
FB_MAKE_HASH(prize_description)
FB_MAKE_HASH(intensity)
FB_MAKE_HASH(added_to_attachment_menu)
FB_MAKE_HASH(has_custom_certificate)
FB_MAKE_HASH(value)
FB_MAKE_HASH(poll)
FB_MAKE_HASH(close_date)
FB_MAKE_HASH(photos)
FB_MAKE_HASH(request_title)
FB_MAKE_HASH(bot_is_member)
FB_MAKE_HASH(can_invite_users)
FB_MAKE_HASH(request_location)
FB_MAKE_HASH(file_unique_id)
FB_MAKE_HASH(menu_button)
FB_MAKE_HASH(error_message)
FB_MAKE_HASH(sticker)
FB_MAKE_HASH(can_add_web_page_previews)
FB_MAKE_HASH(new_chat_title)
FB_MAKE_HASH(contact)
FB_MAKE_HASH(opening_minute)
FB_MAKE_HASH(connected_website)
FB_MAKE_HASH(new_chat_photo)
FB_MAKE_HASH(colors)
FB_MAKE_HASH(sender_chat_id)
FB_MAKE_HASH(secret)
FB_MAKE_HASH(file_hash)
FB_MAKE_HASH(currency)
FB_MAKE_HASH(inline_query)
FB_MAKE_HASH(author_signature)
FB_MAKE_HASH(proximity_alert_radius)
FB_MAKE_HASH(member_limit)
FB_MAKE_HASH(text)
FB_MAKE_HASH(animation)
FB_MAKE_HASH(from_request)
FB_MAKE_HASH(premium_subscription_month_count)
FB_MAKE_HASH(first_name)
FB_MAKE_HASH(order_info)
FB_MAKE_HASH(vcard)
FB_MAKE_HASH(via_bot)
FB_MAKE_HASH(bio)
FB_MAKE_HASH(allow_sending_without_reply)
FB_MAKE_HASH(chat)
FB_MAKE_HASH(is_disabled)
FB_MAKE_HASH(data)
FB_MAKE_HASH(permissions)
FB_MAKE_HASH(emoji_status_custom_emoji_id)
FB_MAKE_HASH(until_date)
FB_MAKE_HASH(edit_date)
FB_MAKE_HASH(invite_link)
FB_MAKE_HASH(channel_chat_created)
FB_MAKE_HASH(total_voter_count)
FB_MAKE_HASH(data_hash)
FB_MAKE_HASH(boost_count)
FB_MAKE_HASH(message)
FB_MAKE_HASH(show_alert)
FB_MAKE_HASH(question)
FB_MAKE_HASH(amount)
FB_MAKE_HASH(custom_title)
FB_MAKE_HASH(general_forum_topic_unhidden)
FB_MAKE_HASH(photo_file_id)
FB_MAKE_HASH(chat_id)
FB_MAKE_HASH(thumbnail_mime_type)
FB_MAKE_HASH(protect_content)
FB_MAKE_HASH(has_public_winners)
FB_MAKE_HASH(point)
FB_MAKE_HASH(gif_url)
FB_MAKE_HASH(username)
FB_MAKE_HASH(is_big)
FB_MAKE_HASH(question_parse_mode)
FB_MAKE_HASH(is_personal)
FB_MAKE_HASH(suggested_tip_amounts)
FB_MAKE_HASH(users_shared)
FB_MAKE_HASH(bot_username)
FB_MAKE_HASH(secret_token)
FB_MAKE_HASH(traveler)
FB_MAKE_HASH(ip_address)
FB_MAKE_HASH(telegram_payment_charge_id)
FB_MAKE_HASH(max_quantity)
FB_MAKE_HASH(revoke_messages)
FB_MAKE_HASH(offset)
FB_MAKE_HASH(photo)
FB_MAKE_HASH(web_app_query_id)
FB_MAKE_HASH(venue)
FB_MAKE_HASH(query)
FB_MAKE_HASH(audio)
FB_MAKE_HASH(month)
FB_MAKE_HASH(request_id)
FB_MAKE_HASH(edited_business_message)
FB_MAKE_HASH(result)
FB_MAKE_HASH(winners_selection_date)
FB_MAKE_HASH(chat_instance)
FB_MAKE_HASH(force)
FB_MAKE_HASH(last_name)
FB_MAKE_HASH(from_chat_id)
FB_MAKE_HASH(is_revoked)
FB_MAKE_HASH(disable_edit_message)
FB_MAKE_HASH(thumbnail_url)
FB_MAKE_HASH(document)
FB_MAKE_HASH(allowed_updates)
FB_MAKE_HASH(mask_position)
FB_MAKE_HASH(chat_member)
FB_MAKE_HASH(business_intro)
FB_MAKE_HASH(is_member)
FB_MAKE_HASH(channel_post)
FB_MAKE_HASH(hide_url)
FB_MAKE_HASH(credentials)
FB_MAKE_HASH(is_bot)
FB_MAKE_HASH(update_id)
FB_MAKE_HASH(via_chat_folder_invite_link)
FB_MAKE_HASH(request_chat)
FB_MAKE_HASH(max_reaction_count)
FB_MAKE_HASH(small_file_unique_id)
FB_MAKE_HASH(can_change_info)
FB_MAKE_HASH(distance)
FB_MAKE_HASH(can_read_all_group_messages)
FB_MAKE_HASH(boosts)
FB_MAKE_HASH(poll_answer)
FB_MAKE_HASH(general_forum_topic_hidden)
FB_MAKE_HASH(google_place_type)
FB_MAKE_HASH(entities)
FB_MAKE_HASH(custom_emoji_sticker_set_name)
FB_MAKE_HASH(birthdate)
FB_MAKE_HASH(mpeg4_file_id)
FB_MAKE_HASH(provider_data)
FB_MAKE_HASH(total_amount)
FB_MAKE_HASH(inline_keyboard)
FB_MAKE_HASH(format)
FB_MAKE_HASH(need_shipping_address)
FB_MAKE_HASH(thumbnail_width)
FB_MAKE_HASH(sender_chat)
FB_MAKE_HASH(request_name)
FB_MAKE_HASH(can_be_edited)
FB_MAKE_HASH(giveaway_completed)
FB_MAKE_HASH(quote_parse_mode)
FB_MAKE_HASH(foursquare_id)
FB_MAKE_HASH(boost)
FB_MAKE_HASH(element_hash)
FB_MAKE_HASH(thumbnail_height)
FB_MAKE_HASH(video_chat_started)
FB_MAKE_HASH(mime_type)
FB_MAKE_HASH(provider_token)
FB_MAKE_HASH(can_send_audios)
FB_MAKE_HASH(via_join_request)
FB_MAKE_HASH(message_reaction)
FB_MAKE_HASH(can_manage_topics)
FB_MAKE_HASH(can_join_groups)
FB_MAKE_HASH(can_send_polls)
FB_MAKE_HASH(edited_message)
FB_MAKE_HASH(can_manage_video_chats)
FB_MAKE_HASH(sender_user_name)
FB_MAKE_HASH(available_reactions)
FB_MAKE_HASH(voter_count)
FB_MAKE_HASH(forum_topic_closed)
FB_MAKE_HASH(source)
FB_MAKE_HASH(prefer_large_media)
FB_MAKE_HASH(timeout)
FB_MAKE_HASH(prefer_small_media)
FB_MAKE_HASH(migrate_to_chat_id)
FB_MAKE_HASH(creates_join_request)
FB_MAKE_HASH(needs_repainting)
FB_MAKE_HASH(web_app_data)
FB_MAKE_HASH(day)
FB_MAKE_HASH(video_note)
FB_MAKE_HASH(foursquare_type)
FB_MAKE_HASH(can_send_voice_notes)
FB_MAKE_HASH(can_send_photos)
FB_MAKE_HASH(can_restrict_members)
FB_MAKE_HASH(business_location)
FB_MAKE_HASH(option_ids)
FB_MAKE_HASH(sticker_type)
FB_MAKE_HASH(payload)
FB_MAKE_HASH(profile_background_custom_emoji_id)
FB_MAKE_HASH(shipping_query)
FB_MAKE_HASH(is_video)
FB_MAKE_HASH(country_code)
FB_MAKE_HASH(prices)
FB_MAKE_HASH(shipping_option_id)
FB_MAKE_HASH(next_offset)
FB_MAKE_HASH(short_description)
FB_MAKE_HASH(custom_emoji_ids)
FB_MAKE_HASH(has_private_forwards)
FB_MAKE_HASH(status)
FB_MAKE_HASH(request_username)
FB_MAKE_HASH(for_channels)
FB_MAKE_HASH(video_file_id)
FB_MAKE_HASH(input_field_placeholder)
FB_MAKE_HASH(post_code)
FB_MAKE_HASH(retry_after)
FB_MAKE_HASH(can_post_stories)
FB_MAKE_HASH(web_app)
FB_MAKE_HASH(name)
FB_MAKE_HASH(can_reply)
FB_MAKE_HASH(video_chat_scheduled)
FB_MAKE_HASH(is_inverted)
FB_MAKE_HASH(chats)
FB_MAKE_HASH(last_synchronization_error_date)
FB_MAKE_HASH(profile_accent_color_id)
FB_MAKE_HASH(sender_user)
FB_MAKE_HASH(request_write_access)
FB_MAKE_HASH(audio_duration)
FB_MAKE_HASH(ok)
FB_MAKE_HASH(total_count)
FB_MAKE_HASH(one_time_keyboard)
FB_MAKE_HASH(invoice_payload)
FB_MAKE_HASH(removed_chat_boost)
FB_MAKE_HASH(type)
FB_MAKE_HASH(shipping_query_id)
FB_MAKE_HASH(inline_query_id)
FB_MAKE_HASH(audio_url)
FB_MAKE_HASH(top_color)
FB_MAKE_HASH(height)
FB_MAKE_HASH(phone_number)
FB_MAKE_HASH(forum_topic_reopened)
FB_MAKE_HASH(my_chat_member)
FB_MAKE_HASH(emoji_status_expiration_date)
FB_MAKE_HASH(front_side)
FB_MAKE_HASH(message_thread_id)
FB_MAKE_HASH(city)
FB_MAKE_HASH(certificate)
FB_MAKE_HASH(message_ids)
FB_MAKE_HASH(closing_minute)
FB_MAKE_HASH(file_id)
FB_MAKE_HASH(quote_entities)
FB_MAKE_HASH(chat_is_channel)
FB_MAKE_HASH(voice_url)
FB_MAKE_HASH(icon_color)
FB_MAKE_HASH(login_url)
FB_MAKE_HASH(button)
FB_MAKE_HASH(description)
FB_MAKE_HASH(remove_caption)
FB_MAKE_HASH(mpeg4_height)
FB_MAKE_HASH(state)
FB_MAKE_HASH(language)
FB_MAKE_HASH(giveaway_message)
FB_MAKE_HASH(can_manage_chat)
FB_MAKE_HASH(pre_checkout_query_id)
FB_MAKE_HASH(file_date)
FB_MAKE_HASH(file_hashes)
FB_MAKE_HASH(boost_id)
FB_MAKE_HASH(unrestrict_boost_count)
FB_MAKE_HASH(accent_color_id)
FB_MAKE_HASH(chat_shared)
FB_MAKE_HASH(user_chat_id)
FB_MAKE_HASH(opening_hours)
FB_MAKE_HASH(gif_width)
FB_MAKE_HASH(action)
FB_MAKE_HASH(horizontal_accuracy)
FB_MAKE_HASH(icon_custom_emoji_id)
FB_MAKE_HASH(is_unclaimed)
FB_MAKE_HASH(is_anonymous)
FB_MAKE_HASH(user_administrator_rights)
FB_MAKE_HASH(is_manual)
FB_MAKE_HASH(has_protected_content)
FB_MAKE_HASH(email)
FB_MAKE_HASH(new_chat_members)
FB_MAKE_HASH(expiration_date)
FB_MAKE_HASH(media_group_id)
FB_MAKE_HASH(user_is_bot)
FB_MAKE_HASH(gif_duration)
FB_MAKE_HASH(video_height)
FB_MAKE_HASH(has_restricted_voice_and_video_messages)
FB_MAKE_HASH(message_reaction_count)
FB_MAKE_HASH(performer)
FB_MAKE_HASH(button_text)
FB_MAKE_HASH(actor_chat)
FB_MAKE_HASH(business_connection)
FB_MAKE_HASH(reply_parameters)
FB_MAKE_HASH(length)
FB_MAKE_HASH(user_is_premium)
FB_MAKE_HASH(old_sticker)
FB_MAKE_HASH(voice_file_id)
FB_MAKE_HASH(deleted_business_messages)
FB_MAKE_HASH(origin)
FB_MAKE_HASH(pending_update_count)
FB_MAKE_HASH(sender_business_bot)
FB_MAKE_HASH(premium_animation)
FB_MAKE_HASH(forward_origin)
FB_MAKE_HASH(can_delete_stories)
FB_MAKE_HASH(open_period)
FB_MAKE_HASH(duration)
FB_MAKE_HASH(has_visible_history)
FB_MAKE_HASH(quote)
FB_MAKE_HASH(label)
FB_MAKE_HASH(photo_height)
FB_MAKE_HASH(hash)
FB_MAKE_HASH(effect_id)
FB_MAKE_HASH(chat_boost)
FB_MAKE_HASH(address)
FB_MAKE_HASH(reply_to_message)
FB_MAKE_HASH(score)
FB_MAKE_HASH(proximity_alert_triggered)
FB_MAKE_HASH(business_message)
FB_MAKE_HASH(big_file_id)
FB_MAKE_HASH(caption)
FB_MAKE_HASH(group_chat_created)
FB_MAKE_HASH(street_line1)
FB_MAKE_HASH(need_name)
FB_MAKE_HASH(is_forum)
FB_MAKE_HASH(story)
FB_MAKE_HASH(explanation_entities)
FB_MAKE_HASH(voter_chat)
FB_MAKE_HASH(file_name)
FB_MAKE_HASH(send_email_to_provider)
FB_MAKE_HASH(document_url)
FB_MAKE_HASH(is_automatic_forward)
FB_MAKE_HASH(game_short_name)
FB_MAKE_HASH(photo_url)
FB_MAKE_HASH(sticker_file_id)
FB_MAKE_HASH(file_size)
FB_MAKE_HASH(callback_game)
FB_MAKE_HASH(result_id)
FB_MAKE_HASH(is_primary)
FB_MAKE_HASH(video)
FB_MAKE_HASH(supergroup_chat_created)
FB_MAKE_HASH(rights)
FB_MAKE_HASH(question_entities)
FB_MAKE_HASH(new_reaction)
FB_MAKE_HASH(giveaway_message_id)
FB_MAKE_HASH(is_enabled)
FB_MAKE_HASH(translation)
FB_MAKE_HASH(can_post_messages)
FB_MAKE_HASH(explanation_parse_mode)
FB_MAKE_HASH(web_app_name)
FB_MAKE_HASH(reply_to_story)
FB_MAKE_HASH(has_media_spoiler)
FB_MAKE_HASH(can_pin_messages)
FB_MAKE_HASH(background_custom_emoji_id)
FB_MAKE_HASH(cache_time)
FB_MAKE_HASH(date)
FB_MAKE_HASH(quote_position)
FB_MAKE_HASH(position)
FB_MAKE_HASH(dice)
FB_MAKE_HASH(linked_chat_id)
FB_MAKE_HASH(invoice)
FB_MAKE_HASH(is_blurred)
FB_MAKE_HASH(sticker_set_name)
FB_MAKE_HASH(location)
FB_MAKE_HASH(errors)
FB_MAKE_HASH(small_file_id)
FB_MAKE_HASH(time_zone_name)
FB_MAKE_HASH(pay)
FB_MAKE_HASH(commands)
FB_MAKE_HASH(users)
}  // namespace tg_apih

namespace tg_cmdh {
FB_MAKE_HASH(unpinAllChatMessages)
FB_MAKE_HASH(getChatAdministrators)
FB_MAKE_HASH(setMyShortDescription)
FB_MAKE_HASH(setMyDefaultAdministratorRights)
FB_MAKE_HASH(sendMessage)
FB_MAKE_HASH(getMyDefaultAdministratorRights)
FB_MAKE_HASH(sendPoll)
FB_MAKE_HASH(sendPhoto)
FB_MAKE_HASH(restrictChatMember)
FB_MAKE_HASH(getForumTopicIconStickers)
FB_MAKE_HASH(forwardMessages)
FB_MAKE_HASH(sendMediaGroup)
FB_MAKE_HASH(answerWebAppQuery)
FB_MAKE_HASH(sendVoice)
FB_MAKE_HASH(revokeChatInviteLink)
FB_MAKE_HASH(setStickerKeywords)
FB_MAKE_HASH(setChatPhoto)
FB_MAKE_HASH(deleteMyCommands)
FB_MAKE_HASH(getUpdates)
FB_MAKE_HASH(forwardMessage)
FB_MAKE_HASH(getChatMember)
FB_MAKE_HASH(getMyCommands)
FB_MAKE_HASH(editChatInviteLink)
FB_MAKE_HASH(closeGeneralForumTopic)
FB_MAKE_HASH(setStickerMaskPosition)
FB_MAKE_HASH(editMessageLiveLocation)
FB_MAKE_HASH(editGeneralForumTopic)
FB_MAKE_HASH(sendDocument)
FB_MAKE_HASH(sendVideo)
FB_MAKE_HASH(copyMessages)
FB_MAKE_HASH(getWebhookInfo)
FB_MAKE_HASH(setChatStickerSet)
FB_MAKE_HASH(uploadStickerFile)
FB_MAKE_HASH(setChatAdministratorCustomTitle)
FB_MAKE_HASH(answerCallbackQuery)
FB_MAKE_HASH(getMyDescription)
FB_MAKE_HASH(setStickerSetTitle)
FB_MAKE_HASH(setMessageReaction)
FB_MAKE_HASH(setChatTitle)
FB_MAKE_HASH(createForumTopic)
FB_MAKE_HASH(setGameScore)
FB_MAKE_HASH(sendAnimation)
FB_MAKE_HASH(unbanChatSenderChat)
FB_MAKE_HASH(addStickerToSet)
FB_MAKE_HASH(deleteMessage)
FB_MAKE_HASH(sendContact)
FB_MAKE_HASH(replaceStickerInSet)
FB_MAKE_HASH(setChatMenuButton)
FB_MAKE_HASH(setMyName)
FB_MAKE_HASH(setPassportDataErrors)
FB_MAKE_HASH(setChatDescription)
FB_MAKE_HASH(closeForumTopic)
FB_MAKE_HASH(getMe)
FB_MAKE_HASH(banChatMember)
FB_MAKE_HASH(setStickerPositionInSet)
FB_MAKE_HASH(close)
FB_MAKE_HASH(unpinChatMessage)
FB_MAKE_HASH(getStickerSet)
FB_MAKE_HASH(sendDice)
FB_MAKE_HASH(createNewStickerSet)
FB_MAKE_HASH(setChatPermissions)
FB_MAKE_HASH(answerPreCheckoutQuery)
FB_MAKE_HASH(sendGame)
FB_MAKE_HASH(editMessageMedia)
FB_MAKE_HASH(deleteChatStickerSet)
FB_MAKE_HASH(sendVideoNote)
FB_MAKE_HASH(logOut)
FB_MAKE_HASH(sendSticker)
FB_MAKE_HASH(editForumTopic)
FB_MAKE_HASH(reopenGeneralForumTopic)
FB_MAKE_HASH(refundStarPayment)
FB_MAKE_HASH(copyMessage)
FB_MAKE_HASH(sendLocation)
FB_MAKE_HASH(deleteMessages)
FB_MAKE_HASH(getChat)
FB_MAKE_HASH(leaveChat)
FB_MAKE_HASH(getUserChatBoosts)
FB_MAKE_HASH(editMessageCaption)
FB_MAKE_HASH(approveChatJoinRequest)
FB_MAKE_HASH(answerShippingQuery)
FB_MAKE_HASH(sendChatAction)
FB_MAKE_HASH(createInvoiceLink)
FB_MAKE_HASH(setWebhook)
FB_MAKE_HASH(reopenForumTopic)
FB_MAKE_HASH(unpinAllForumTopicMessages)
FB_MAKE_HASH(deleteStickerSet)
FB_MAKE_HASH(getMyShortDescription)
FB_MAKE_HASH(deleteWebhook)
FB_MAKE_HASH(editMessageReplyMarkup)
FB_MAKE_HASH(answerInlineQuery)
FB_MAKE_HASH(setStickerEmojiList)
FB_MAKE_HASH(getGameHighScores)
FB_MAKE_HASH(getUserProfilePhotos)
FB_MAKE_HASH(createChatInviteLink)
FB_MAKE_HASH(deleteForumTopic)
FB_MAKE_HASH(deleteChatPhoto)
FB_MAKE_HASH(getChatMemberCount)
FB_MAKE_HASH(setMyCommands)
FB_MAKE_HASH(getFile)
FB_MAKE_HASH(getChatMenuButton)
FB_MAKE_HASH(getBusinessConnection)
FB_MAKE_HASH(declineChatJoinRequest)
FB_MAKE_HASH(getMyName)
FB_MAKE_HASH(sendVenue)
FB_MAKE_HASH(banChatSenderChat)
FB_MAKE_HASH(deleteStickerFromSet)
FB_MAKE_HASH(pinChatMessage)
FB_MAKE_HASH(promoteChatMember)
FB_MAKE_HASH(editMessageText)
FB_MAKE_HASH(stopPoll)
FB_MAKE_HASH(getCustomEmojiStickers)
FB_MAKE_HASH(unbanChatMember)
FB_MAKE_HASH(hideGeneralForumTopic)
FB_MAKE_HASH(unhideGeneralForumTopic)
FB_MAKE_HASH(stopMessageLiveLocation)
FB_MAKE_HASH(sendAudio)
FB_MAKE_HASH(setStickerSetThumbnail)
FB_MAKE_HASH(setCustomEmojiStickerSetThumbnail)
FB_MAKE_HASH(exportChatInviteLink)
FB_MAKE_HASH(setMyDescription)
FB_MAKE_HASH(unpinAllGeneralForumTopicMessages)
FB_MAKE_HASH(sendInvoice)
}  // namespace tg_cmdh
