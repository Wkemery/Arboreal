startup<-read.table("/home/wyatt/Documents/Arboreal/Source/Data/create_tag_time.txt", header = TRUE)

#I see no relationships here

startup<-startup[startup$time < 0.1,]
plot(time~files, data = startup, main = "CreateTag: Time ~ Files")
plot(time~tags, data = startup, main = "CreateTag: Time ~ Tags")


fit<-lm(time~tags + files, data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Tags", main="Tags Residual Plot")



